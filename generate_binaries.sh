for f in configs/*.json; do
    if [ "$f" == "configs/champsim_config.json" ]; then
        continue
    fi
    echo "Building ${f}"
    ./config.sh configs/champsim_config.json ${f} && make
done

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
# ./config.sh configs/champsim_config.json configs/ipcp_l1d.json && make # DPC3

### L2C Prefetchers:
# ./config.sh configs/champsim_config.json configs/no_l2c.json && make
# ./config.sh configs/champsim_config.json configs/next_line_l2c.json && make
# ./config.sh configs/champsim_config.json configs/ip_stride_l2c.json && make
# ./config.sh configs/champsim_config.json configs/spp_dev_l2c.json && make

# ./config.sh configs/champsim_config.json configs/ampm_l2c.json && make # Pythia
# ./config.sh configs/champsim_config.json configs/bingo_l2c.json && make # Pythia
# ./config.sh configs/champsim_config.json configs/bop_l2c.json && make # Pythia
# ./config.sh configs/champsim_config.json configs/dspatch_l2c.json && make # Pythia
# ./config.sh configs/champsim_config.json configs/ipcp_l2c.json && make # DPC3
# ./config.sh configs/champsim_config.json configs/mlop_l2c.json && make # Pythia
# ./config.sh configs/champsim_config.json configs/sandbox_l2c.json && make # Pythia
# ./config.sh configs/champsim_config.json configs/scooby_l2c.json && make # Pythia
# ./config.sh configs/champsim_config.json configs/sms_l2c.json && make # Pythia
# ./config.sh configs/champsim_config.json configs/spp_ppf_dev_l2c.json && make
# ./config.sh configs/champsim_config.json configs/streamer_l2c.json && make # Pythia

# LLC Prefetchers:
# ./config.sh configs/champsim_config.json configs/no_llc.json && make
# ./config.sh configs/champsim_config.json configs/next_line_llc.json && make
# ./config.sh configs/champsim_config.json configs/ip_stride_llc.json && make

# LLC Replacement Policies:
# ./config.sh configs/champsim_config.json configs/lru.json && make
# ./config.sh configs/champsim_config.json configs/ship.json && make
# ./config.sh configs/champsim_config.json configs/srrip.json && make
# ./config.sh configs/champsim_config.json configs/drrip.json && make
# ./config.sh configs/champsim_config.json configs/mockingjay.json && make