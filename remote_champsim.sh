#!/bin/bash
exec > >(tee jobs.log) 2>&1

usage() {
    echo "Usage: $0 suite --option1 value1 --option2 value2 ..."
    echo "Example: $0 spec --ifetch-buffer 64 --fetch-width 8 --decode-width 8"
    exit 1
}

job_submition() {
    # Define the file path
    data_directory="results/${suite_name}/${1}-data"
    file_path="jobs/run.job"

    # Backup the original file
    backup_file="${file_path}.bak"
    cp "$file_path" "$backup_file"

    # Check if the temporary line is in the file
    if ! grep -q "## champsim_command" "$file_path"; then
        echo "Error: The ChampSim temporary line is not in the file '${file_path}'."
        exit -1
    fi

    # Replace
    sed -i "s|#account|${ACCOUNT}|" "$file_path"
    sed -i "s|#email|${EMAIL}|" "$file_path"
    sed -i "s|## champsim_command|champsim_command=$champsim_command|" "$file_path"

    # Submit the job
    job_name="${1}-${trace_name}-${binary_name}"

    mkdir -p "${data_directory}"

    if [ ! -s "${data_directory}/${trace_name}-${binary_name}.txt" ]; then
        echo "Submitting ${job_name}"
        output=$(sbatch -Q -J "${job_name}" --output=/dev/null --error=/dev/null "$file_path" 2>&1)
        exit_code=$?

        if [[ $exit_code -ne 0 || "$output" == *"your account's balance is not sufficient"* ]]; then
            echo "Error: Failed to submit job for ${job_name}."
            echo "$output"
        fi
    else
        output_file="${data_directory}/${trace_name}-${binary_name}.txt"
        if ! grep -q "ChampSim completed all CPUs" "${output_file}"; then
            echo "Check this file (missing completion message): ${output_file}"
        fi
    fi

    # Revert the changes by restoring the backup
    cp "$backup_file" "$file_path"
}

start_watcher() {
    # Define the file path
    file_path="jobs/watcher.job"

    # Backup the original file
    backup_file="${file_path}.bak"
    cp "$file_path" "$backup_file"

    # Replace
    sed -i "s|#account|${ACCOUNT}|" "$file_path"
    sed -i "s|#email|${EMAIL}|" "$file_path"

    if [ $(squeue -u $USER | grep watcher | wc -l) -eq 0 ]; then
        echo "Starting Watcher"
        sbatch -Q -J watcher --output=/dev/null --error=/dev/null $file_path
    fi

    # Revert the changes by restoring the backup
    cp "$backup_file" "$file_path"
}

# Check if enough arguments are provided
if [ "$#" -lt 1 ]; then
    usage
fi

# Extract the suite
suite=$1
shift

# Parse the additional options
options=()
while [[ $# -gt 0 ]]; do
    case $1 in
    --ifetch-buffer | --fetch-width | --decode-width | --dispatch-width | --execute-width | --retire-width | --rob-size)
        options+=("$1 $2")
        shift 2
        ;;
    *)
        echo "Error: Unknown option '$1'"
        usage
        ;;
    esac
done

# Validate suite
if [ "${suite}" != "GAPS" ] && [ "${suite}" != "SPEC" ] && [ "${suite}" != "LCF" ] && [ "${suite}" != "ALL" ]; then
    echo "Error: Invalid suite '${suite}'."
    usage
fi

if [ -z "${options[*]}" ]; then
    echo "Running ${suite} suite(s) with default options."
else
    echo "Running ${suite} suite(s) with options: ${options[*]}"
fi

# Start watcher
start_watcher

w=10000000
s=100000000
max_jobs=490

if [ "${suite}" == "ALL" ]; then
    trace_files=$(ls ../new_trace_format/*/*.gz)
else
    trace_files=$(ls ../new_trace_format/${suite}/*.gz)
fi

# Start processing traces
for trace in ${trace_files}; do
    trace_name=$(basename $trace)
    trace_name=${trace_name%.gz}
    echo "Processing ${trace_name}"

    if [[ "$trace_name" == *"500.perlbench"* || "$trace_name" == *"520.omnetpp"* || "$trace_name" == *"523.xalancbmk"* ]]; then
        continue
    fi

    # If ALL, extract suite name from the path; else use the provided suite
    if [ "${suite}" == "ALL" ]; then
        suite_name=$(basename "$(dirname "${trace}")")
    else
        suite_name="${suite}"
    fi

    for binary in $(ls bin/*); do
        binary_name=$(basename $binary)

        # Construct the configuration suffix based on user-provided options
        config_suffix=""
        for option in "${options[@]}"; do
            key=$(echo $option | awk '{print $1}' | sed 's/--//')
            value=$(echo $option | awk '{print $2}')
            config_suffix="${config_suffix}_${key}${value}"
        done

        # Construct the ChampSim command
        champsim_command="'${binary} --warmup-instructions ${w} --simulation-instructions ${s} \
        ${options[@]} ${trace} > results/${suite_name}/cp-data/${trace_name}-${binary_name}${config_suffix}.txt'"
        job_submition "cp"

        # Wait if the job queue is too long
        num_jobs=$(squeue -u $USER | wc -l)
        while [ $num_jobs -ge $max_jobs ]; do
            sleep 60
            num_jobs=$(squeue -u $USER | wc -l)
        done

        # Repeat for wp simulation
        champsim_command="'${binary} --warmup-instructions ${w} --simulation-instructions ${s} \
        ${options[@]} --wrong-path ${trace} > results/${suite_name}/wp-data/${trace_name}-${binary_name}${config_suffix}.txt'"
        job_submition "wp"

        while [ $num_jobs -ge $max_jobs ]; do
            sleep 60
            num_jobs=$(squeue -u $USER | wc -l)
        done

        # Repeat for wpa simulation
        champsim_command="'${binary} --warmup-instructions ${w} --simulation-instructions ${s} \
        ${options[@]} --wrong-path --wpa ${trace} > results/${suite_name}/wpa-data/${trace_name}-${binary_name}${config_suffix}.txt'"
        job_submition "wpa"

        while [ $num_jobs -ge $max_jobs ]; do
            sleep 60
            num_jobs=$(squeue -u $USER | wc -l)
        done
    done
done
