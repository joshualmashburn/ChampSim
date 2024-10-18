#include "cache.h"
#include <iostream>

#define MAX_PQ_SIZE 32
#define MAX_RQ_SIZE 64
#define MAX_MSHR_SIZE 16
#define MAX_NUM_SET 64
#define MAX_NUM_WAY 8

using namespace std;
//geomean 1.287
//geomean 1.211 if only MMA8 (set MAXFNL  to 0)
//geomean 1.165 if only FNL5


#define AHEADPRED
#define DISTAHEAD 8

//a pseudo RNG (largely sufficient for the simulator)
static uint64_t RANDSEED = 0x3f79a17b4;
uint64_t
MYRANDOM ()
{
  uint64_t X = (RANDSEED >> 7) * 0x9745931;
  if (X == RANDSEED)
    X++;
  RANDSEED = X;
  return (RANDSEED & 127);
}

#define LOGMULTSIZE  (0)	//to test other sizes of predictors:  +1 doubles the size of MMA table and FNL tables


#define MMA_FILT_SIZE 17	// 17 entries in the MMA FILTER
static uint64_t PREVPRED[MMA_FILT_SIZE];	// the MMA prefetches
#define DISTAHEADMAX 40
static uint64_t PREVADDR[DISTAHEADMAX + 1];	//to memorize the previous addresses missing the I-Shadow cache


/// All variables  for FNL
#define MAXFNL 5		// 3 to 6  reaches approximately the same performance, but slightly more accesses to L2 with larger MAXFNL
#define PERIODRESET 8192
#define FNL_NBENTRIES (1<< (16+LOGMULTSIZE ))
static int WorthPF[FNL_NBENTRIES];
static int Touched[FNL_NBENTRIES];
static int ptReset;


#define NBWAYISHADOW 3
#define SIZESHADOWICACHE (64*NBWAYISHADOW)
static uint64_t ShadowICache[64][NBWAYISHADOW];


////////////////////////////

#define FITERFNLON
#define NBWAYFILTERFNL 4
#define SIZEWAYFILTERFNL 32
#define SIZEFILTERFNL (SIZEWAYFILTERFNL*NBWAYFILTERFNL)
static uint64_t JUSTNLPREFETCH[SIZEWAYFILTERFNL][NBWAYFILTERFNL];
static int ptFilterFnl;
void
JustFnl (uint64_t Block)
{
//mark the block has just being fetch or prefetch (FIFO management)
  uint64_t set = (Block & (SIZEWAYFILTERFNL - 1));
  uint64_t tag = (Block / SIZEWAYFILTERFNL) & ((1 << 15) - 1);

  for (int i = NBWAYFILTERFNL - 1; i > 0; i--)
    JUSTNLPREFETCH[set][i] = JUSTNLPREFETCH[set][i - 1];
  JUSTNLPREFETCH[set][0] = tag;

}

bool
WasNotJustFnl (uint64_t Block)
{
//check if the block has been fetched or prefetched
  uint64_t prev = Block - 1;
  int set = (prev & (SIZEWAYFILTERFNL - 1));
  uint64_t tag = (prev / SIZEWAYFILTERFNL) & ((1 << 15) - 1);
  for (int i = 0; i < NBWAYFILTERFNL; i++)
    if (JUSTNLPREFETCH[set][i] == tag)
      return false;
  return true;
}

///////////////////////////////

bool
IsInIShadow (uint64_t Block, bool Insert)
{
// also manage replacement policy if Insert = true
  int Hit = -1;
  int set = Block & 63;
  int tag = (Block >> 6) & ((1 << 15) - 1);
  for (int i = 0; i < NBWAYISHADOW; i++)
    if (tag == ShadowICache[set][i])
      {
	Hit = i;
	break;
      }
  if (Insert)
    {
      // Simple solution for software management of LRU 
      int Max = (Hit != -1) ? Hit : NBWAYISHADOW - 1;
      for (int i = Max; i > 0; i--)
	ShadowICache[set][i] = ShadowICache[set][i - 1];
      ShadowICache[set][0] = tag;
    }
  return (Hit != -1);
}

////////////////////

#define LOGTAGNEXTMISS 12	// 12 bit tags

class PredictMiss
{
public:
  uint64_t * Ntag;		// 12 bits
  uint64_t *NBlock;		// 58 bits
  int8_t *U;			// 1 bit for replacement and confidence
  int SIZEWAYNEXTMISS;
  int LOGWAYNEXTMISS;
  int distahead;
  void init (int X, int LOGSIZE)
  {
    LOGWAYNEXTMISS = LOGSIZE + LOGMULTSIZE;
    SIZEWAYNEXTMISS = (1 << LOGWAYNEXTMISS);
    Ntag = new uint64_t[4 * SIZEWAYNEXTMISS];
    NBlock = new uint64_t[4 * SIZEWAYNEXTMISS];
    U = new int8_t[4 * SIZEWAYNEXTMISS];
    for (int i = 0; i < 4 * SIZEWAYNEXTMISS; i++)
      {
	U[i] = 0;
	Ntag[i] = 0;
	NBlock[i] = 0;
      }
    distahead = X;
  }
  uint64_t AheadPredict (uint64_t Addr)
  {
    //  manage  the table as a skewed cache :-)
    int index[4];
    int A = Addr & (SIZEWAYNEXTMISS - 1);
    int B = (Addr >> LOGWAYNEXTMISS) & (SIZEWAYNEXTMISS - 1);
    for (int i = 0; i < 4; i++)
      {
	index[i] = (A ^ B) + (i << LOGWAYNEXTMISS);
	A = (A >> 3) + ((A & 7) << (LOGWAYNEXTMISS - 3));
      }


    uint64_t tag = (Addr >> LOGWAYNEXTMISS) & ((1 << LOGTAGNEXTMISS) - 1);
    int NHIT = -1;
    for (int i = 0; i < 4; i++)
      {
	if (Ntag[index[i]] == tag)
	  NHIT = i;
      }
    if (NHIT == -1)
      return (0);
    uint64_t X = 0;
    if (U[index[NHIT]] > 0)	// there were at least two  misses on the same block
      X = NBlock[index[NHIT]];
    return (X);
  }

  void LinkAhead (uint64_t Block, uint64_t PrevAddr, uint8_t Hit)
  {
    // Fill the table  on I-cache miss
    if (Hit)
      return;
    uint64_t PrevBlock = (PrevAddr >> (LOG2_BLOCK_SIZE - 2));
    int index[4];
    int A = PrevAddr & (SIZEWAYNEXTMISS - 1);
    int B = (PrevAddr >> LOGWAYNEXTMISS) & (SIZEWAYNEXTMISS - 1);
    for (int i = 0; i < 4; i++)
      {
	index[i] = (A ^ B) + (i << LOGWAYNEXTMISS);
	A = (A >> 3) + ((A & 7) << (LOGWAYNEXTMISS - 3));
      }
    uint64_t tag = (PrevAddr >> LOGWAYNEXTMISS) & ((1 << LOGTAGNEXTMISS) - 1);
    int NHIT = -1;
    for (int i = 0; i < 4; i++)
      {
	if (Ntag[index[i]] == tag)
	  NHIT = i;
      }

    if (NHIT != -1)
      {
	if (NBlock[index[NHIT]] == Block)
	  {
	    //increase the confidence
	    U[index[NHIT]] = 1;
	  }
	else
	  {
	    //reset confidence
	    U[index[NHIT]] = 0;
	    NBlock[index[NHIT]] = Block;
	  }
      }
    else
      {				// let us try to allocate a new entry
	int X = MYRANDOM () & 3;
	for (int i = 0; i < 4; i++)
	  {
	    if (U[index[X]] == 0)
	      {
		NHIT = X;
		break;
	      };
	    X = (X + 1) & 3;
	  }
	if (NHIT == -1)
	  {
	    // decay some entry
	    if ((MYRANDOM () & 3) == 0)
	      {
		U[index[X]] = 0;
	      }
	  }
	if (NHIT != -1)
	  {
	    //allocate the entry
	    NBlock[index[NHIT]] = Block;
	    Ntag[index[NHIT]] = tag;
	  }
      }

  }
};

PredictMiss AHEAD;

#define 	PrefCodeBlock(X) prefetch_line ((X)<<LOG2_BLOCK_SIZE, true, 0)
// prefetch  works on  blocks

/////////////////////////////////
void
CACHE::prefetcher_initialize ()
{
  cout << "CPU " << cpu << " L1I next line prefetcher" << endl;
  AHEAD.init (DISTAHEAD, 11);
}


void
CACHE::prefetcher_branch_operate (uint64_t ip, uint8_t branch_type,
				       uint64_t branch_target)
{

}

////////////////////
uint32_t CACHE::prefetcher_cache_operate(uint64_t v_addr, uint64_t ip, uint8_t cache_hit, bool prefetch_hit, uint8_t type, uint32_t metadata_in)
{
  //cout << "access v_addr: 0x" << hex << v_addr << dec << endl;
  uint64_t Block = v_addr >> LOG2_BLOCK_SIZE;
  int index = Block & (FNL_NBENTRIES - 1);
  bool ShadowMiss = (!IsInIShadow (Block, 1));
  uint64_t AheadPredictedBlock = 0;
// prefetch is triggered only on misses on the Shadow I-cache
  if (ShadowMiss)
    {

// The FNL prefetcher
/////// Manage if it is worth prefetching next block
      int previndex = (index - 1) & (FNL_NBENTRIES - 1);
      Touched[index] = 1;
      if (Touched[previndex])
	{			//the previous block was read not so long ago: it was worth prefetching this block
	  WorthPF[previndex] = 3;
	}

      for (int i = ptReset; i < ptReset + (FNL_NBENTRIES / PERIODRESET); i++)
// Once a block has become worth prefetching, it keeps this status for at least three intervals of PERIODRESET I-Shadow misses
	{

	  if (Touched[i])
	    if (WorthPF[i] > 0)
	      WorthPF[i]--;
	  Touched[i] = 0;
	}
      ptReset += (FNL_NBENTRIES / PERIODRESET);
      ptReset &= (FNL_NBENTRIES - 1);

////////
// Next-line prefetch
      if (WorthPF[index] > 0)
	{
	  bool NotJustAHEAD = true;
//verify that the block has not been already prefetched by MMA recently
	  for (int i = MMA_FILT_SIZE - 1; i >= 0; i--)
	    if (PREVPRED[i] == Block)
	      {
		NotJustAHEAD = false;
		break;
	      }
	  if (NotJustAHEAD)

	    {
	      for (int i = 1; i <= MAXFNL; i++)
		{
		  uint64_t pf_Block = Block + i;
//if Block B-1 was accessed recently one has only to prefetch Block block+FNL
		  if ((WasNotJustFnl (Block)) || (i == MAXFNL))
		    {
		      PrefCodeBlock (pf_Block);
		    }
		  if (WorthPF[(index + i) & (FNL_NBENTRIES - 1)] == 0)
		    break;
		}
	    }
#ifdef  FITERFNLON
	  JustFnl (Block);
#endif
	}
/////// END OF THE FNL prefetcher
#ifdef AHEADPRED
      AheadPredictedBlock = AHEAD.AheadPredict (v_addr >> 2);
      if (AheadPredictedBlock != 0)
	{
	  bool NotJustMMA = true;
	  for (int i = MMA_FILT_SIZE - 1; i >= 0; i--)
	    {
	      if (PREVPRED[i] == (AheadPredictedBlock))
		{
		  NotJustMMA = false;
		  break;
		}
	    }
	  if (NotJustMMA)
	    NotJustMMA = !IsInIShadow (AheadPredictedBlock, 0);
	  if (!NotJustMMA)
	    AheadPredictedBlock = 0;
	  if (NotJustMMA)
	    {
	      int index = (AheadPredictedBlock) & (FNL_NBENTRIES - 1);
// avoid issing prefetch the block if the previous block was prefetched

	      PrefCodeBlock (AheadPredictedBlock);

	      if (WorthPF[index] > 0)
		{
		  for (int i = 1; i <= MAXFNL; i++)
		    {
		      uint64_t pf_Block = AheadPredictedBlock + i;
		      if ((WasNotJustFnl (AheadPredictedBlock))
			  || (i == MAXFNL))
			PrefCodeBlock (pf_Block);
		      if (WorthPF[(index + i) & (FNL_NBENTRIES - 1)] == 0)
			break;
		    }
#ifdef  FITERFNLON
		  JustFnl (AheadPredictedBlock);
#endif
		}

	    }
	}
      if ((Block != (PREVADDR[0] >> 4) + 1) || (MAXFNL == 0))
	{			// Link Block to the address of the block that missed DISTAHEAD+1 before
	  AHEAD.LinkAhead (Block, PREVADDR[AHEAD.distahead], cache_hit);
	}


      for (int i = DISTAHEADMAX; i > 0; i--)
	PREVADDR[i] = PREVADDR[i - 1];
      PREVADDR[0] = v_addr >> 2;


      if (AheadPredictedBlock != 0);
      {
	for (int i = MMA_FILT_SIZE - 1; i >= 1; i--)
	  PREVPRED[i] = PREVPRED[i - 1];
	PREVPRED[0] = AheadPredictedBlock;
      }
#endif
    }
  return metadata_in;
}

void
CACHE::prefetcher_cycle_operate ()
{

}


uint32_t CACHE::prefetcher_cache_fill(uint64_t v_addr, uint32_t set, uint32_t way, uint8_t prefetch, uint64_t evicted_v_addr, uint32_t metadata_in)
{
  //cout << hex << "fill: 0x" << v_addr << dec << " " << set << " " << way << " " << (uint32_t)prefetch << " " << hex << "evict: 0x" << evicted_v_addr << dec << endl;
  return metadata_in;
}

void
CACHE::prefetcher_final_stats ()
{
  printf ("Prefetcher storage:\n Miss Ahead Prediction Table %d bytes\n ",
	  72 * 4 * AHEAD.SIZEWAYNEXTMISS / 8);
  printf ("I-Shadow cache %d bytes\n", (SIZESHADOWICACHE * (15 + 2)) / 8);
  printf ("Touched + WorthPF tables %d bytes \n", (FNL_NBENTRIES * 3) / 8);
  printf ("MMA filter %d bytes \n", (MMA_FILT_SIZE * 58) / 8);
  printf ("FNL filter %d bytes \n", (SIZEFILTERFNL * (15 + 2)) / 8);
  printf ("TOTAL PREFETCHER STORAGE SIZE: %d bytes\n",
	  (72 * 4 * AHEAD.SIZEWAYNEXTMISS / 8) +
	  ((SIZESHADOWICACHE * (15 + 2)) / 8) + ((FNL_NBENTRIES * 3) / 8) +
	  ((MMA_FILT_SIZE * 58) / 8) + ((SIZEFILTERFNL * (15 + 2) / 8)));
  cout << "CPU " << cpu << " L1I next line prefetcher final stats" << endl;
}

uint32_t CACHE::prefetcher_prefetch_hit(uint64_t addr, uint64_t ip, uint32_t metadata_in) { return metadata_in; }

void CACHE::prefetcher_broadcast_bw(uint64_t bw_level) {}

void CACHE::prefetcher_broadcast_ipc(uint64_t ipc) {}

void CACHE::prefetcher_broadcast_acc(uint64_t acc_level) {}