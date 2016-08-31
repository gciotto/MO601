/*
 * A new PINTOOL which calculates the routine with the biggest number of instructions
 * for every thread composing the a program.
 *
 * Author: Gustavo CIOTTO PINTON
 * Computer Architecture II MO601B
 */

#include <iostream>
#include <fstream>
#include <string.h>
#include "pin.H"

#define MAX_ROUTINES_PER_THREAD 50

#define STATIC_INS 1

/* Output file handler */
ofstream OutFile;

/* thread counter */
INT32 number_threads = 0;

/* KNOB handles input arguments. According to the reference page,
 * KNOB_MODE_WRITEONCE indicates that, in case of multiple appearances of the argument
 * on the command line, the program must consider just one of them (single value, single write) */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "most-used.out", "specify output file name");

PIN_LOCK lock;

/* TLS storage for threads  */
static TLS_KEY tls_key;

class Routine_info {


public:
	Routine_info() : _ins_count (0), _next (NULL) {}
	UINT64 _ins_count;
	string _name;
	Routine_info* _next;
};


class Thread_info {

public:
	Thread_info(): _routine_head(NULL), _routine_count (0) {}

	Routine_info* _routine_head;
	UINT32 _routine_count;
};



/* Function of the 'type' THREAD_START_CALLBACK(THREADID threadIndex, CONTEXT *ctxt, INT32 flags, VOID *v),
 * according to the API reference */
VOID initThreadData (THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v) {

    OutFile << "initThreadData: Requesting lock...";

    PIN_GetLock(&lock, threadid+1);

    OutFile << "Ok" << endl;

    number_threads++;

    OutFile << "initThreadData: Releasing lock...";

    PIN_ReleaseLock(&lock);

    OutFile << "Ok" << endl;

    OutFile << "initThreadData: Creating data structure for thread #" << (number_threads - 1) << "...";

    Thread_info* tdata = new Thread_info;

    OutFile << "Ok" << endl ;

    PIN_SetThreadData(tls_key, tdata, threadid);
}

/* This function is called before every instruction is executed */
VOID incrementCounter(THREADID _thread_id, string _name) {

	Thread_info* _t_info = static_cast<Thread_info*> (PIN_GetThreadData(tls_key, _thread_id));

	for (Routine_info* t =  _t_info->_routine_head; t; t = t->_next)
		if (!t->_name.compare(_name)) {
			t->_ins_count++;
			break;
		}

}

VOID saveRoutineObjectStatic(THREADID _thread_id, string _name, UINT32 c) {


    Thread_info* _t_info = static_cast<Thread_info*> (PIN_GetThreadData(tls_key, _thread_id));


	if (_t_info) {

		UINT8 hasFound = 0;
		Routine_info* t;
		for (t =  _t_info->_routine_head; t; t = t->_next)
			if (!t->_name.compare(_name)) {
				hasFound = 1;
				break;
			}

		if (!hasFound) {

			Routine_info* _new_routine = new Routine_info;

			_new_routine->_name = _name;

			/* Adds new routine to the beginning of the linked list */
			_new_routine->_next = _t_info->_routine_head;
			_t_info->_routine_head = _new_routine;
			_new_routine->_ins_count = c;
		}
		else t->_ins_count += c;
	}

}

VOID saveRoutineObject(THREADID _thread_id, string _name) {


    Thread_info* _t_info = static_cast<Thread_info*> (PIN_GetThreadData(tls_key, _thread_id));

	if (_t_info) {

		UINT8 hasFound = 0;
		Routine_info* t;
		for (t =  _t_info->_routine_head; t; t = t->_next)
			if (!t->_name.compare(_name)) {
				hasFound = 1;
				break;
			}

		if (!hasFound) {

			Routine_info* _new_routine = new Routine_info;

			_new_routine->_name = _name;


			/* Adds new routine to the beginning of the linked list */
			_new_routine->_next = _t_info->_routine_head;
			_t_info->_routine_head = _new_routine;
		}
	}

}

/* Function of the 'type' RTN_INSTRUMENT_CALLBACK(RTN rtn, VOID *v). */
VOID initRoutineCallback(RTN rtn, VOID *v) {

    RTN_Open(rtn);

    string* _rtn_name = new string (RTN_Name(rtn));


    // RTN_NumIns 	(  	RTN  	rtn 	 )

    if (STATIC_INS)
    	RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)saveRoutineObjectStatic, IARG_THREAD_ID, IARG_PTR , _rtn_name, IARG_UINT32, RTN_NumIns(rtn),  IARG_END);

    else {

		RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)saveRoutineObject, IARG_THREAD_ID, IARG_PTR , _rtn_name, IARG_END);

		//For each instruction of the routine
		for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)incrementCounter, IARG_THREAD_ID, IARG_PTR, _rtn_name, IARG_END);
    }

    RTN_Close(rtn);



}

/* Callback function called when application is exiting */
VOID endTool(INT32 code, VOID *v)
{
    // Write to a file since cout and cerr maybe closed by the application
    OutFile << "Total number of threads = " << number_threads << endl;

    for (INT32 i = 0 ; i < number_threads; i++) {

    	Thread_info* _t_info = static_cast<Thread_info*> (PIN_GetThreadData(tls_key, i));

    	OutFile << "Thread #" << decstr(i) << endl;

    	for (Routine_info* t = _t_info->_routine_head; t; t = t->_next)
    			OutFile << "|----- " << t->_name << " with " << t->_ins_count << " static instructions. " << endl;
    		else
    		if (STATIC_INS)
    			OutFile << "|----- " << t->_name << " with " << t->_ins_count << " executed instructions. " << endl;
    	OutFile.flush();

    }

    OutFile.close();
}

/* Default help message */
INT32 Usage()
{
    PIN_ERROR( "This Pintool prints a trace of memory addresses\n"
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* Main function. It opens a file handler and adds an instrument function
 * after all instruction in program  */
int main(int argc, char *argv[])
{
    if (PIN_Init(argc, argv)) return Usage();

    PIN_InitSymbols();

    OutFile.open(KnobOutputFile.Value().c_str(), ofstream::out | ofstream::app);

    OutFile << "Creating lock...";

    /* Initialize the lock */
    PIN_InitLock(&lock);

    OutFile << "Ok" << endl;

    OutFile << "Creating TLS key...";

    /* Obtain  a key for TLS storage. */
    tls_key = PIN_CreateThreadDataKey(0);

    OutFile << "Ok" << endl;

    OutFile << "Running PIN_AddThreadStartFunction...";

    PIN_AddThreadStartFunction(initThreadData, 0);

    OutFile << "Ok" << endl;

    OutFile << "Running RTN_AddInstrumentFunction...";

    RTN_AddInstrumentFunction(initRoutineCallback, 0);

    OutFile << "Ok" << endl;

    PIN_AddFiniFunction(endTool, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}

