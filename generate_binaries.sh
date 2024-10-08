# for f in configs/*.json; do
#     if [ "$f" == "configs/champsim_config.json" ]; then
#         continue
#     fi
#     echo "Building ${f}"
#     ./config.sh configs/champsim_config.json ${f} && make
# done

# L1I prefetchers:
# ./config.sh configs/champsim_config.json configs/barsa_config.json && make
# ./config.sh configs/champsim_config.json configs/bip_config.json && make
# ./config.sh configs/champsim_config.json configs/djolt_config.json && make
# # ./config.sh configs/champsim_config.json configs/epi_config.json && make
# ./config.sh configs/champsim_config.json configs/fnlmma_config.json && make
# ./config.sh configs/champsim_config.json configs/fnlmma_new_config.json && make
# # ./config.sh configs/champsim_config.json configs/ISCA_Entangling_4Ke_config.json && make
# # ./config.sh configs/champsim_config.json configs/ISCA_Entangling_4Ke_opt_bb_split_config.json && make
# ./config.sh configs/champsim_config.json configs/mana_config.json && make
# ./config.sh configs/champsim_config.json configs/next_line_config.json && make
# ./config.sh configs/champsim_config.json configs/no_config.json && make
# ./config.sh configs/champsim_config.json configs/pips_config.json && make
# ./config.sh configs/champsim_config.json configs/tap_config.json && make

# L1D prefetchers:
# ./config.sh configs/champsim_config.json configs/berti_config.json && make
./config.sh configs/champsim_config.json configs/ipcp_config.json && make