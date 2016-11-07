#include <iostream>
#include <fstream>

#include "pin.H"
#include "pinplay.H"

#include "sim_api.h"

#define WARMUP 100e6

/* PinPlay requirements */
PINPLAY_ENGINE pinplay_engine;
KNOB<BOOL> KnobPinPlayLogger(KNOB_MODE_WRITEONCE, 
                      "pintool", "log", "0",
                      "Activate the pinplay logger");
KNOB<BOOL> KnobPinPlayReplayer(KNOB_MODE_WRITEONCE, 
                      "pintool", "replay", "0",
                      "Activate the pinplay replayer");

UINT64 counter = WARMUP;


LOCALFUN VOID Fini(int code, VOID * v)
{
	SimRoiEnd();
}

LOCALFUN ADDRINT verify_counter()
{
    return counter;
}

LOCALFUN VOID decrement_counter() {

    counter--;

    if (!counter)
        SimRoiStart();
}

LOCALFUN VOID initInsCallBack(INS ins, VOID *v) {
	
    // verify_counter() is called for every instruction executed
    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) verify_counter, IARG_END);
    
    // PrintIp() is called only when counter > 0.
    INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) decrement_counter, IARG_END);

}

GLOBALFUN int main(int argc, char *argv[])
{
	PIN_Init(argc, argv);

	pinplay_engine.Activate(argc, argv, KnobPinPlayLogger, KnobPinPlayReplayer);

	INS_AddInstrumentFunction(initInsCallBack, 0);

	PIN_AddFiniFunction(Fini, 0);

	PIN_StartProgram();

	return 0;
}


