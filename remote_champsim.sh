job_submition() {
    # Define the file path
    data_directory="results/${suite}/${1}-data"
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

    sbatch -Q -J ${job_name} --output=/dev/null --error=/dev/null $file_path

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

if [ "${1}" == "gap" ]; then
    suite="gap"
elif [ "${1}" == "spec" ]; then
    suite="spec"
elif [ "${1}" == "lcf" ]; then
    suite="lcf"
else
    echo "Error: Invalid suite '${1}'."
    exit -1
fi

echo "Running ${suite} suite"
start_watcher

warmup_instructions=10000000
simulation_instructions=100000000
for trace in $(ls ../${suite}/*.gz); do
    trace_name=$(basename $trace)
    trace_name=${trace_name%.gz}
    echo "Processing ${trace_name}"

    for binary in $(ls bin/*); do
        binary_name=$(basename $binary)
        echo "Running ${binary_name}"

        champsim_command="'${binary} --warmup-instructions ${warmup_instructions} --simulation-instructions ${simulation_instructions} ${trace} > results/${suite}/cp-data/${trace_name}-${binary_name}.txt'"
        job_submition "cp"

        num_jobs=$(squeue -u $USER | wc -l)
        while [ $num_jobs -ge 500 ]; do
            sleep 60
            num_jobs=$(squeue -u $USER | wc -l)
        done

        champsim_command="'${binary} --warmup-instructions ${warmup_instructions} --simulation-instructions ${simulation_instructions} --wrong-path ${trace} > results/${suite}/wp-data/${trace_name}-${binary_name}.txt'"
        job_submition "wp"

        while [ $num_jobs -ge 500 ]; do
            sleep 60
            num_jobs=$(squeue -u $USER | wc -l)
        done

        champsim_command="'${binary} --warmup-instructions ${warmup_instructions} --simulation-instructions ${simulation_instructions} --wrong-path --wpa ${trace} > results/${suite}/wpa-data/${trace_name}-${binary_name}.txt'"
        job_submition "wpa"

        num_jobs=$(squeue -u $USER | wc -l)
        while [ $num_jobs -ge 500 ]; do
            sleep 60
            num_jobs=$(squeue -u $USER | wc -l)
        done
    done
done
