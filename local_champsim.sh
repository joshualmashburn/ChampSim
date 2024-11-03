mkdir -p cp-data
mkdir -p wp-data

./config.sh configs/champsim_config.json configs/no.json && make

trace=../traces/557.xz.gz
trace_name=$(basename $trace)
trace_name=${trace_name%.gz}

warmup=1000000
sim=10000000

echo "Running CP default"
./bin/champsim-default --warmup-instructions $warmup --simulation-instructions $sim ${trace} >cp-data/${trace_name}-default.txt
# echo "Running CP scooby"
# ./bin/champsim-l2c-scooby --warmup-instructions $warmup --simulation-instructions $sim ${trace} >cp-data/${trace_name}-scooby.txt

echo "Running WP default"
./bin/champsim-default --warmup-instructions $warmup --simulation-instructions $sim --wrong-path ${trace} >wp-data/${trace_name}-default.txt
echo "Running WPA"
./bin/champsim-default --warmup-instructions $warmup --simulation-instructions $sim --wrong-path --wpa ${trace} >wp-data/${trace_name}-wpa.txt

# gdb -q -ex=r --args ./bin/champsim-l1i-epi --warmup-instructions 1000000 --simulation-instructions 10000000 --wrong-path ../new-traces/tomcat.gz
