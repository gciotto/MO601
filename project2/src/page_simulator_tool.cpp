#include <iostream>
#include <fstream>

#include "pin.H"
#include "pinplay.H"

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

/* Output file handler */
ofstream OutFile;

/* KNOB handles input arguments. According to the reference page,
 * KNOB_MODE_WRITEONCE indicates that, in case of multiple appearances of the argument
 * on the command line, the program must consider just one of them (single value, single write) */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
		"o", "most-used.out", "specify output file name");

/* PinPlay requirements */
PINPLAY_ENGINE pinplay_engine;
KNOB<BOOL> KnobPinPlayLogger(KNOB_MODE_WRITEONCE, 
                      "pintool", "log", "0",
                      "Activate the pinplay logger");
KNOB<BOOL> KnobPinPlayReplayer(KNOB_MODE_WRITEONCE, 
                      "pintool", "replay", "0",
                      "Activate the pinplay replayer");


LOCALFUN VOID Fini(int code, VOID * v)
{
	OutFile << itlb_4m;
	OutFile << dtlb_4m;
	OutFile << itlb_4k;
	OutFile << dtlb_4k;

	OutFile << "Instruction memory accesses:" << endl;
	OutFile << "|--- (4MB) " << itlb_4m.Misses() * TABLE_TREE_LEVEL << endl;
	OutFile << "|--- (4KB) " << itlb_4k.Misses() * TABLE_TREE_LEVEL << endl;


	OutFile << "Data memory accesses:" << endl;
	OutFile << "|--- (4MB) " << dtlb_4m.Misses() * TABLE_TREE_LEVEL << endl;
	OutFile << "|--- (4KB) " << dtlb_4k.Misses() * TABLE_TREE_LEVEL << endl;
}

LOCALFUN VOID checkDataTLB(ADDRINT addr, UINT32 size, BOOL access_type) {

	CACHE_BASE::ACCESS_TYPE _access = access_type ? CACHE_BASE::ACCESS_TYPE_LOAD : CACHE_BASE::ACCESS_TYPE_STORE;

	if (size <= 4) {

		dtlb_4k.AccessSingleLine(addr, _access);
		dtlb_4m.AccessSingleLine(addr, _access);
	}
	else {

		dtlb_4k.Access(addr, size, _access);
		dtlb_4m.Access(addr, size, _access);
	}

}

LOCALFUN VOID BBL_analysisCallBack (ADDRINT addr, UINT32 size) {

	itlb_4k.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);

	itlb_4m.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);
}

LOCALFUN VOID initTraceCallBack(TRACE trace, VOID *v) {

	for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {

		ADDRINT address = BBL_Address(bbl);

		/* Checks INSTRUCTION TLB */
		BBL_InsertCall (bbl, IPOINT_BEFORE,
						(AFUNPTR) BBL_analysisCallBack,
						IARG_ADDRINT, address,
						IARG_UINT32, BBL_Size(bbl),
						IARG_END);						

		for(INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)) {


			if (INS_IsStandardMemop(ins)) {

				BOOL access_t = INS_IsMemoryRead(ins) ? 0 : 1;

				/* Checks DATA TLB */
				if (INS_IsMemoryRead(ins))
					// only predicated-on memory instructions access D-cache
					INS_InsertPredicatedCall(
							ins, IPOINT_BEFORE, (AFUNPTR) checkDataTLB,
							IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE , IARG_BOOL, access_t,
							IARG_END);					


				if (INS_IsMemoryWrite(ins))
					// only predicated-on memory instructions access D-cache
					INS_InsertPredicatedCall(
							ins, IPOINT_BEFORE, (AFUNPTR) checkDataTLB,
							IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE, IARG_BOOL, access_t,
							IARG_END);
				
			}
		}
	}

}

GLOBALFUN int main(int argc, char *argv[])
{
	PIN_Init(argc, argv);

	pinplay_engine.Activate(argc, argv, KnobPinPlayLogger, KnobPinPlayReplayer);

	OutFile.open(KnobOutputFile.Value().c_str(), ofstream::out);

	TRACE_AddInstrumentFunction(initTraceCallBack, 0);

	PIN_AddFiniFunction(Fini, 0);

	PIN_StartProgram();

	return 0;
}


