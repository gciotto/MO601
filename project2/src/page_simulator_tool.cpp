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
    // 1st level instruction cache: 32 kB, 64 B lines, 8-way associative
    const UINT32 cacheSize = 32*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 8;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR L1::CACHE il1("L1 Instruction Cache", L1::cacheSize, L1::lineSize, L1::associativity);
LOCALVAR L1::CACHE dl1("D1 Instruction Cache", L1::cacheSize, L1::lineSize, L1::associativity);

namespace UL2
{
    // 2nd level unified cache: 256 kB, 64 B lines, 8-way associative
    const UINT32 cacheSize = 256*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 8;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR UL2::CACHE ul2("L2 Unified Cache", UL2::cacheSize, UL2::lineSize, UL2::associativity);

namespace UL3
{
    // 2nd level unified cache: 4 MB, 64 B lines, 16-way associative
    const UINT32 cacheSize = 4*MEGA;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 16;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR UL3::CACHE ul3("L3 Unified Cache", UL3::cacheSize, UL3::lineSize, UL3::associativity);

UINT64 instruction_misses, data_misses;

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

	OutFile << il1;
	OutFile << dl1;

	OutFile << ul2;

	OutFile << ul3;

	OutFile << "Instruction_Memory:" << endl;
	OutFile << "Total Misses: "  << instruction_misses << endl;

	OutFile << "Data_memory:" << endl;
	OutFile << "Total Misses: "  << data_misses << endl;
}

LOCALFUN VOID checkDataTLB(ADDRINT addr, UINT32 size, BOOL access_type) {

	CACHE_BASE::ACCESS_TYPE _access = access_type ? CACHE_BASE::ACCESS_TYPE_LOAD : CACHE_BASE::ACCESS_TYPE_STORE;

	BOOL hit = 0;

	dtlb_4k.Access(addr, size, _access);
	dtlb_4m.Access(addr, size, _access);

	hit = dl1.Access(addr, size, _access);

	if (!hit) {

		hit = ul2.Access(addr, size, _access);

		if (!hit){ 
		
			ul3.Access(addr, size, _access);
			data_misses++;
		}
	}

}

LOCALFUN VOID BBL_analysisCallBack (ADDRINT addr, UINT32 size) {

	BOOL hit;

	itlb_4k.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);
	itlb_4m.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);

	hit = il1.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);

	if (!hit) {

		hit = ul2.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);

		if (!hit) {
			
			ul3.Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);
			instruction_misses++;
		}
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

	instruction_misses = data_misses = 0;

	TRACE_AddInstrumentFunction(initTraceCallBack, 0);

	PIN_AddFiniFunction(Fini, 0);

	PIN_StartProgram();

	return 0;
}


