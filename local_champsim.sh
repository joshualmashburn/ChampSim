mkdir -p cp-data
mkdir -p wp-data

# for trace in $(ls ../new-traces/*.gz); do
#     trace_name=$(basename $trace)
#     trace_name=${trace_name%.gz}
#     echo "Processing ${trace_name}"

#     for binary in $(ls bin/*); do
#         binary_name=$(basename $binary)
#         echo "Running ${binary_name}"

#         ${binary} --warmup-instructions 10000 --simulation-instructions 20000 $trace >cp-data/${trace_name}-${binary_name}.txt
#         ${binary} --warmup-instructions 10000 --simulation-instructions 20000 --wrong-path $trace >wp-data/${trace_name}-${binary_name}.txt
#     done
# done

trace=../traces/557.xz.gz
trace_name=$(basename $trace)
trace_name=${trace_name%.gz}

warmup=1000000
sim=5000000

echo "Running CP no"
./bin/champsim-l2c-no --warmup-instructions $warmup --simulation-instructions $sim ${trace} >cp-data/${trace_name}-no.txt
echo "Running CP dspatch"
./bin/champsim-l2c-dspatch --warmup-instructions $warmup --simulation-instructions $sim ${trace} >cp-data/${trace_name}-dspatch.txt

echo "Running WP no"
./bin/champsim-l2c-no --warmup-instructions $warmup --simulation-instructions $sim --wrong-path ${trace} >wp-data/${trace_name}-no.txt
echo "Running WP dspatch"
./bin/champsim-l2c-dspatch --warmup-instructions $warmup --simulation-instructions $sim --wrong-path ${trace} >wp-data/${trace_name}-dspatch.txt

# gdb -q -ex=r --args ./bin/champsim-l1i-epi --warmup-instructions 1000000 --simulation-instructions 10000000 --wrong-path ../new-traces/tomcat.gz
