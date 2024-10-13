# for f in configs/*.json; do
#     if [ "$f" == "configs/champsim_config.json" ]; then
#         continue
#     fi
#     echo "Building ${f}"
#     ./config.sh configs/champsim_config.json ${f} && make
# done

# L1I prefetchers:
# ./config.sh configs/champsim_config.json configs/barsa.json && make
# ./config.sh configs/champsim_config.json configs/bip.json && make
# ./config.sh configs/champsim_config.json configs/djolt.json && make
# ./config.sh configs/champsim_config.json configs/epi.json && make
# ./config.sh configs/champsim_config.json configs/fnlmma.json && make
# ./config.sh configs/champsim_config.json configs/fnlmma_new.json && make
# # ./config.sh configs/champsim_config.json configs/ISCA_Entangling_4Ke.json && make
# # ./config.sh configs/champsim_config.json configs/ISCA_Entangling_4Ke_opt_bb_split.json && make
# ./config.sh configs/champsim_config.json configs/mana.json && make
# ./config.sh configs/champsim_config.json configs/next_line_instr.json && make
# ./config.sh configs/champsim_config.json configs/no_instr.json && make
# ./config.sh configs/champsim_config.json configs/pips.json && make
# ./config.sh configs/champsim_config.json configs/tap.json && make

# L1D prefetchers:
# ./config.sh configs/champsim_config.json configs/no.json && make
# ./config.sh configs/champsim_config.json configs/next_line.json && make
# ./config.sh configs/champsim_config.json configs/berti.json && make
./config.sh configs/champsim_config.json configs/ip_stride.json && make
# ./config.sh configs/champsim_config.json configs/ipcp.json && make