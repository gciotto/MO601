#include <iostream>
#include <fstream>

#include "pin.H"

typedef UINT32 CACHE_STATS; // type of cache hit/miss counters

#include "pin_cache.H"

#define TABLE_TREE_LEVEL 3

enum page_size {
	PAGE_4KB,
	PAGE_4MB,

	N_PAGE
};

namespace TLB_4K
{
// TLB: 4 kB pages, 512 entries, fully associative
const UINT32 lineSize = 4*KILO;
const UINT32 cacheSize = 512 * lineSize;
const UINT32 associativity = 512;
const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

const UINT32 max_sets = cacheSize / (lineSize * associativity);
const UINT32 max_associativity = associativity;

typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR TLB_4K::CACHE itlb_4k("ITLB_4K", TLB_4K::cacheSize, TLB_4K::lineSize, TLB_4K::associativity);
LOCALVAR TLB_4K::CACHE dtlb_4k("DTLB_4K", TLB_4K::cacheSize, TLB_4K::lineSize, TLB_4K::associativity);

namespace TLB_4M
{
// TLB: 4 MB pages, 512 entries, fully associative
const UINT32 lineSize = 4*MEGA;
const UINT32 cacheSize = 512 * lineSize;
const UINT32 associativity = 512;
const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

const UINT32 max_sets = cacheSize / (lineSize * associativity);
const UINT32 max_associativity = associativity;

typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR TLB_4M::CACHE itlb_4m("ITLB_4M", TLB_4M::cacheSize, TLB_4M::lineSize, TLB_4M::associativity);
LOCALVAR TLB_4M::CACHE dtlb_4m("DTLB_4M", TLB_4M::cacheSize, TLB_4M::lineSize, TLB_4M::associativity);

LOCALVAR UINT32 imem_access[N_PAGE];
LOCALVAR UINT32 dmem_access[N_PAGE];

/* Output file handler */
ofstream OutFile;

/* KNOB handles input arguments. According to the reference page,
 * KNOB_MODE_WRITEONCE indicates that, in case of multiple appearances of the argument
 * on the command line, the program must consider just one of them (single value, single write) */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
		"o", "most-used.out", "specify output file name");


LOCALFUN VOID Fini(int code, VOID * v)
{
	OutFile << itlb_4m;
	OutFile << dtlb_4m;
	OutFile << itlb_4k;
	OutFile << dtlb_4k;

	unsigned int i;

	OutFile << "Instruction memory accesses:" << endl;
	for (i = 0; i < N_PAGE; i++) {
		OutFile << "|---" << imem_access[i] << endl;
	}

	OutFile << "Data memory accesses:" << endl;
	for (i = 0; i < N_PAGE; i++) {
		OutFile << "|---" << dmem_access[i] << endl;
	}
}

LOCALFUN VOID checkInstructionTLB(ADDRINT addr) {

	bool hit;

	hit = itlb_4k.AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD);

	if (!hit) imem_access[PAGE_4KB] += TABLE_TREE_LEVEL;

	hit = itlb_4m.AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD);

	if (!hit) imem_access[PAGE_4MB] += TABLE_TREE_LEVEL;

}

LOCALFUN VOID checkDataTLB(ADDRINT addr) {

	bool hit;

	hit = dtlb_4k.AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD);

	if (!hit) dmem_access[PAGE_4KB] += TABLE_TREE_LEVEL;

	hit = dtlb_4m.AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD);

	if (!hit) dmem_access[PAGE_4MB] += TABLE_TREE_LEVEL;

}

LOCALFUN VOID initInstrCallBack(INS ins, VOID *v)
{
	// all instruction fetches access I-cache
	INS_InsertCall(
			ins, IPOINT_BEFORE, (AFUNPTR) checkInstructionTLB,
			IARG_INST_PTR,
			IARG_END);

	if (INS_IsMemoryRead(ins) && INS_IsStandardMemop(ins))
	{

		// only predicated-on memory instructions access D-cache
		INS_InsertPredicatedCall(
				ins, IPOINT_BEFORE, (AFUNPTR) checkDataTLB,
				IARG_MEMORYREAD_EA,
				IARG_END);
	}

	if (INS_IsMemoryWrite(ins) && INS_IsStandardMemop(ins))	{

		// only predicated-on memory instructions access D-cache
		INS_InsertPredicatedCall(
				ins, IPOINT_BEFORE, (AFUNPTR) checkDataTLB,
				IARG_MEMORYWRITE_EA,
				IARG_END);
	}
}

GLOBALFUN int main(int argc, char *argv[])
{
	PIN_Init(argc, argv);

	OutFile.open(KnobOutputFile.Value().c_str(), ofstream::out);

	unsigned int i;
	for (i = 0; i < N_PAGE; i++) {
		imem_access[i] = 0;
		dmem_access[i] = 0;
	}

	INS_AddInstrumentFunction(initInstrCallBack, 0);
	PIN_AddFiniFunction(Fini, 0);

	PIN_StartProgram();

	return 0;
}

