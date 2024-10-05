mkdir -p cp-data
mkdir -p wp-data

for trace in $(ls ../new-traces/*.gz); do
    trace_name=$(basename $trace)
    trace_name=${trace_name%.gz}
    echo "Processing ${trace_name}"

    for binary in $(ls bin/*); do
        binary_name=$(basename $binary)
        echo "Running ${binary_name}"

        ${binary} --warmup-instructions 10000 --simulation-instructions 20000 $trace >cp-data/${trace_name}-${binary_name}.txt
        ${binary} --warmup-instructions 10000 --simulation-instructions 20000 --wrong-path $trace >wp-data/${trace_name}-${binary_name}.txt
    done
done

# ./bin/champsim-djolt --warmup-instructions 100000 --simulation-instructions 200000 --wrong-path ../new-traces/tomcat.gz >wp-data/tomcat.txt
# gdb -q --tui -ex=r --args ./bin/champsim-djolt --warmup-instructions 100000 --simulation-instructions 200000 --wrong-path ../new-traces/tomcat.gz
