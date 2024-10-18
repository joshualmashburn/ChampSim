#include "cache.h"

void CACHE::prefetcher_initialize() {}

uint32_t CACHE::prefetcher_cache_operate(uint64_t addr, uint64_t ip, uint8_t cache_hit, bool useful_prefetch, uint8_t type, uint32_t metadata_in)
{
  return metadata_in;
}

uint32_t CACHE::prefetcher_cache_fill(uint64_t addr, uint32_t set, uint32_t way, uint8_t prefetch, uint64_t evicted_addr, uint32_t metadata_in)
{
  return metadata_in;
}

void CACHE::prefetcher_cycle_operate() {}

void CACHE::prefetcher_final_stats() {}

uint32_t CACHE::prefetcher_prefetch_hit(uint64_t addr, uint64_t ip, uint32_t metadata_in)
{
  return metadata_in;
}

void CACHE::prefetcher_broadcast_bw(uint64_t bw_level) {}

void CACHE::prefetcher_broadcast_ipc(uint64_t ipc) {}

void CACHE::prefetcher_broadcast_acc(uint64_t acc_level) {}