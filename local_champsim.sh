make

mkdir -p cp-data
mkdir -p wp-data

# for trace in $(ls ../traces/*.gz); do
#     trace_name=$(basename $trace)
#     trace_name=${trace_name%.gz}
#     echo "Processing ${trace_name}"

#     ./bin/champsim --warmup-instructions 10000000 --simulation-instructions 20000000 $trace >cp-data/${trace_name}.txt

#     ./bin/champsim --warmup-instructions 10000000 --simulation-instructions 20000000 --wrong-path $trace >wp-data/${trace_name}.txt
# done

./bin/champsim --warmup-instructions 10000000 --simulation-instructions 20000000 --wrong-path ../traces/tomcat.gz >wp-data/tomcat.txt
# gdb -q --tui -ex=r --args ./bin/champsim --warmup-instructions 10000000 --simulation-instructions 20000000 --wrong-path ../traces/tomcat.gz
