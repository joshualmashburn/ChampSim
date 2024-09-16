make

job_submition() {
    # Define the file path
    directory="logs"
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
    job_name="${trace_name}"

    mkdir -p "${directory}"

    sbatch -Q -J ${job_name} --output=${directory}/%x.out --error=${directory}/%x.err $file_path

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
        sbatch -Q -J watcher --output=/dev/null --error=/dev/null $file_path
    fi

    # Revert the changes by restoring the backup
    cp "$backup_file" "$file_path"
}

for trace in $(ls ../wp-traces/*.gz); do
    echo "Processing ${trace}"
    trace_name=$(basename $trace)
    champsim_command="'./bin/champsim --warmup-instructions 10000000 --simulation-instructions 100000000 --wrong-path $trace > logs/${trace_name}.log'"
    job_submition
done

start_watcher
