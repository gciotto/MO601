#include <iostream>
#include <fstream>

#include "pin.H"
#include "pinplay.H"

typedef UINT32 CACHE_STATS; // type of cache hit/miss counters

#include "pin_cache.H"

#define TABLE_TREE_LEVEL 3 

/* TLB 4KB PAGES */
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

/* TLB 4MB PAGES */
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

namespace L1
{
    // 1st level instruction cache: 32 kB, 32 B lines, 32-way associative
    const UINT32 cacheSize = 64*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 64;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_NO_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR L1::CACHE il1_4k("L1 4KB Instruction Cache", L1::cacheSize, L1::lineSize, L1::associativity);
LOCALVAR L1::CACHE dl1_4k("D1 4KB Instruction Cache", L1::cacheSize, L1::lineSize, L1::associativity);
LOCALVAR L1::CACHE il1_4m("L1 4MB Instruction Cache", L1::cacheSize, L1::lineSize, L1::associativity);
LOCALVAR L1::CACHE dl1_4m("D1 4MB Instruction Cache", L1::cacheSize, L1::lineSize, L1::associativity);

namespace UL2
{
    // 2nd level unified cache: 2 MB, 64 B lines, direct mapped
    const UINT32 cacheSize = 4*MEGA;
    const UINT32 lineSize = 128;
    const UINT32 associativity = 1;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);

    typedef CACHE_DIRECT_MAPPED(max_sets, allocation) CACHE;
}
LOCALVAR UL2::CACHE ul2_4k("L2 4KB Unified Cache", UL2::cacheSize, UL2::lineSize, UL2::associativity);
LOCALVAR UL2::CACHE ul2_4m("L2 4MB Unified Cache", UL2::cacheSize, UL2::lineSize, UL2::associativity);

namespace UL3
{
    // 2nd level unified cache: 2 MB, 64 B lines, direct mapped
    const UINT32 cacheSize = 32*MEGA;
    const UINT32 lineSize = 128;
    const UINT32 associativity = 1;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);

    typedef CACHE_DIRECT_MAPPED(max_sets, allocation) CACHE;
}
LOCALVAR UL3::CACHE ul3_4k("L3 4KB Unified Cache", UL3::cacheSize, UL3::lineSize, UL3::associativity);
LOCALVAR UL3::CACHE ul3_4m("L3 4MB Unified Cache", UL3::cacheSize, UL3::lineSize, UL3::associativity);

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

	OutFile << il1_4k;
	OutFile << dl1_4k;

	OutFile << il1_4m;
	OutFile << dl1_4m;

	OutFile << ul2_4k;
	OutFile << ul2_4m;

	OutFile << ul3_4k;
	OutFile << ul3_4m;

}

LOCALFUN VOID checkDataTLB(ADDRINT addr, UINT32 size, BOOL access_type) {

	CACHE_BASE::ACCESS_TYPE _access = access_type ? CACHE_BASE::ACCESS_TYPE_LOAD : CACHE_BASE::ACCESS_TYPE_STORE;

	BOOL hit_4k = 0, hit_4m = 0;

	dtlb_4k.Access(addr, size, _access);
	dtlb_4m.Access(addr, size, _access);

	hit_4k = dl1_4k.Access(addr, size, _access);
	hit_4m = dl1_4m.Access(addr, size, _access);

	if (!hit_4k) {

		hit_4k = ul2_4k.Access(addr, size, _access);
		if (!hit_4k) 
			ul3_4k.Access(addr, size, _access);
	}

	if (!hit_4m) {

		hit_4m = ul2_4m.Access(addr, size, _access);
		if (!hit_4m) 
			ul3_4m.Access(addr, size, _access);			
	}

}

LOCALFUN VOID BBL_analysisCallBack (ADDRINT addr, UINT32 size) {

	BOOL hit_4k, hit_4m;

	itlb_4k.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);
	itlb_4m.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);

	hit_4k = il1_4k.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);
	hit_4m = il1_4m.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);

	if (!hit_4k) {

		hit_4k = ul2_4k.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);

		if (!hit_4k)
			ul3_4k.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);
	}

	if (!hit_4m) {

		hit_4m = ul2_4m.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);

		if (!hit_4m)
			ul3_4m.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);
	}
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


