# for f in configs/*.json; do
#     if [ "$f" == "configs/champsim_config.json" ]; then
#         continue
#     fi
#     echo "Building ${f}"
#     ./config.sh configs/champsim_config.json ${f} && make
# done

### L1I Prefetchers:
# ./config.sh configs/champsim_config.json configs/no.json && make
# ./config.sh configs/champsim_config.json configs/next_line.json && make

# ./config.sh configs/champsim_config.json configs/barsa.json && make
# ./config.sh configs/champsim_config.json configs/bip.json && make
# ./config.sh configs/champsim_config.json configs/djolt.json && make
# ./config.sh configs/champsim_config.json configs/epi.json && make
# ./config.sh configs/champsim_config.json configs/fnlmma.json && make
# ./config.sh configs/champsim_config.json configs/fnlmma_new.json && make
# ./config.sh configs/champsim_config.json configs/mana.json && make
# ./config.sh configs/champsim_config.json configs/pips.json && make
# ./config.sh configs/champsim_config.json configs/tap.json && make

### L1D Prefetchers:
# ./config.sh configs/champsim_config.json configs/no_l1d.json && make
# ./config.sh configs/champsim_config.json configs/next_line_l1d.json && make
# ./config.sh configs/champsim_config.json configs/ip_stride_l1d.json && make

# ./config.sh configs/champsim_config.json configs/berti_l1d.json && make # ChampSim PR

### L2C Prefetchers:
# ./config.sh configs/champsim_config.json configs/no_l2c.json && make
# ./config.sh configs/champsim_config.json configs/next_line_l2c.json && make
# ./config.sh configs/champsim_config.json configs/ip_stride_l2c.json && make

# ./config.sh configs/champsim_config.json configs/sms.json && make
# ./config.sh configs/champsim_config.json configs/bop.json && make
# ./config.sh configs/champsim_config.json configs/next_line_l2c.json && make
# ./config.sh configs/champsim_config.json configs/sandbox.json && make
# ./config.sh configs/champsim_config.json configs/streamer.json && make

# LLC Prefetchers:
# ./config.sh configs/champsim_config.json configs/no_llc.json && make
# ./config.sh configs/champsim_config.json configs/next_line_llc.json && make
# ./config.sh configs/champsim_config.json configs/ip_stride_llc.json && make

# LLC Replacement Policies:
