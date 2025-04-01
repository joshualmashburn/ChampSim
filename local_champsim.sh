#!/bin/bash

rm -rf .csconfig && ./config.sh configs/champsim_config.json configs/no.json
rm -rf bin # && rm -rf temp
mkdir -p temp/cp-data && mkdir -p temp/wp-data && mkdir -p temp/wpa-data
make -j32

w=1000000
s=10000000

# for trace in $(ls ../Traces/TAGE_SC_L/cp-spec/*.gz); do
    trace="../Traces/TAGE_SC_L/cp-spec/505.mcf_r_00.gz"
    trace_name=$(basename $trace)
    trace_name=${trace_name%.gz}
    echo "Running $trace_name"

    echo "Running Correct Path default"
    ./bin/champsim-default --warmup-instructions $w --simulation-instructions $s ${trace} >temp/cp-data/${trace_name}-default.txt &

    echo "Running Wrong Path default"
    ./bin/champsim-default --warmup-instructions $w --simulation-instructions $s --wrong-path ${trace} >temp/wp-data/${trace_name}-default.txt &

    # echo "Running Wrong Path Aware default"
    # ./bin/champsim-default --warmup-instructions $w --simulation-instructions $s --wrong-path --wpa ${trace} >temp/wpa-data/${trace_name}-default.txt &
# done

# sed -n '/Warmup finished/,$p' temp/cp-data/${trace_name}-default.txt > temp.tmp && mv temp.tmp temp/cp-data/${trace_name}-default.txt
# sed -n '/Warmup finished/,$p' temp/wp-data/${trace_name}-default.txt > temp.tmp && mv temp.tmp temp/wp-data/${trace_name}-default.txt
# sed -n '/Warmup finished/,$p' temp/wpa-data/${trace_name}-default.txt > temp.tmp && mv temp.tmp temp/wpa-data/${trace_name}-default.txt