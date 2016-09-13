/*
 * A new PINTOOL which calculates how many times the instructions of each routine of each thread are 
 * executed.
 *
 * Author: Gustavo CIOTTO PINTON
 * Computer Architecture II MO601B
 */

#include <iostream>
#include <fstream>
#include <string.h>
#include "pin.H"

#define MAX_ROUTINES_PER_THREAD 10
#define STATIC_INS 1

/* Output file handler */
ofstream OutFile;

/* Thread counter */
UINT32 number_threads = 0;

/* KNOB handles input arguments. According to the reference page,
 * KNOB_MODE_WRITEONCE indicates that, in case of multiple appearances of the argument
 * on the command line, the program must consider just one of them (single value, single write) */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "most-used.out", "specify output file name");

PIN_LOCK lock;

/* TLS storage for threads  */
static TLS_KEY tls_key;


/* A node containing all information about a thread */
class Thread_node {

public:
	Thread_node(): _id(0), _ins_count (0), _next (NULL) {}

	UINT32 _id;
	UINT64 _ins_count;
	Thread_node* _next;
};


/* Each routine node contains the head and the tail of a linked list which
   contains information about each thread where it is called */
class Routine_node {

public:
	Routine_node() :  _id (0),  _n_threads (0), _threads_head (NULL), _threads_tail (NULL), _next (NULL) {}

	string _name;
	UINT32 _id, _n_threads;
	Thread_node *_threads_head, *_threads_tail;
	Routine_node* _next;
};


Routine_node *_routine_head = NULL, *_routine_tail = NULL;

/* Function of the 'type' THREAD_START_CALLBACK(THREADID threadIndex, CONTEXT *ctxt, INT32 flags, VOID *v),
 * according to the API reference */
VOID initThreadData (THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v) {

    OutFile << "initThreadData: Requesting lock...";

    PIN_GetLock(&lock, threadid+1);

    OutFile << "Ok" << endl;

    number_threads++;

    OutFile << "initThreadData: Releasing lock...";

    PIN_ReleaseLock(&lock);

}

/* This function is called before every instruction is executed */
VOID incrementCounter(THREADID _thread_id, Routine_node* _rtn) {


	if (!_rtn->_threads_head) {

		_rtn->_threads_head = new Thread_node;
		_rtn->_threads_tail = _rtn->_threads_head;

		_rtn->_threads_head->_id = _thread_id;
		_rtn->_threads_head->_ins_count++;

		_rtn->_n_threads++;
		return;
	}

	for (Thread_node* _t = _rtn->_threads_head; _t; _t = _t->_next) {
		if (_t->_id == _thread_id) {
			_t->_ins_count++;
			return;
		}		
	}

	Thread_node* _n = new Thread_node;
	_n->_id = _thread_id;
	_n->_ins_count++;
	
	_rtn->_threads_tail->_next = _n;
	_rtn->_threads_tail = _n;	
}

/* Function of the 'type' RTN_INSTRUMENT_CALLBACK(RTN rtn, VOID *v). This is the instrumentation function and
   it is called just once for each routine to determine which code should be inserted in the given binary. All
   'heavy' processing should be moved from the analysis routine to this function in order to prevent significant 
 	performance loss. 

   In our case, we look for the routine 'rtn' in the list of routines, which can take a large amount of time,
   and leave the adding operation to the analysis routine.
*/
VOID initRoutineCallback(RTN rtn, VOID *v) {

    RTN_Open(rtn);

	if (RTN_Valid(rtn)) {

	    /* Verify if rtn is already in list */

		/* Pin assigns a unique id to each routine - refer to the manual -
		   we use it to identify the routines. */
		UINT32 _rtn_id = RTN_Id(rtn);
		string _rtn_name = RTN_Name(rtn);
		Routine_node* _n;

		/* if _routine_head is NULL, then we should create it. */
		if (!_routine_head) {

			/* create a new linked list */
			_routine_head = new Routine_node;
			_routine_head->_id = _rtn_id;
			_routine_head->_name = _rtn_name;

			_routine_tail = _routine_head;

			_n = _routine_head;



		}
		else { /* List is not empty, verifies if routine is already in it */


			BOOL inList = 0;
			for (Routine_node* _t = _routine_head; _t; _t = _t->_next) 
				if (_t->_id == _rtn_id) {
					_n = _t;
					inList = 1;
					break;
				}

			if (!inList) {
				
				/* If 'rtn' is not found in list, create a new node in the end 
				   end of the linked list. */
				Routine_node* _new = new Routine_node;
				_new->_id = _rtn_id;
				_new->_name = _rtn_name;

				_routine_tail->_next = _new;
				_routine_tail = _new;
				_n = _new;
				
			}

		}

		/* For each valid instruction of the routine, we increment its counter */
		for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)incrementCounter, IARG_THREAD_ID, IARG_PTR, _n, IARG_END);

	}

    RTN_Close(rtn);

}

Routine_node* getAndRemoveBiggestRoutine(UINT32 threadID, UINT64* inscount){

	*inscount = 0;
	Routine_node *biggest = NULL;
	Thread_node* previousBiggestThread = NULL, *biggestThread = NULL;

	for (Routine_node* t = _routine_head; t; t = t->_next) {
		Thread_node *o = NULL;
		for (Thread_node *s = t->_threads_head; s; s = s->_next)  {
			if (s->_id == threadID && s->_ins_count > *inscount)  {

				*inscount = s->_ins_count;
				biggest = t;
				previousBiggestThread = o;
				biggestThread = s;
			}
			o = s;
		}
	}

	if (previousBiggestThread) {
		if (biggestThread)
			previousBiggestThread->_next = biggestThread->_next;
	}
	else 
		if (biggest)
			biggest->_threads_head = biggestThread->_next;
		

	return biggest;

}

/* Callback function called when application is exiting */
VOID endTool(INT32 code, VOID *v)
{
    /* Write to a file since cout and cerr maybe closed by the application */
    OutFile << "Total number of threads = " << number_threads << endl;

    for (UINT32 i = 0 ; i < number_threads; i++) {
	
		UINT64 value;

    	OutFile << "Thread #" << decstr(i) << endl;
		
		Routine_node* biggest =	getAndRemoveBiggestRoutine(i, &value);

		while (biggest) {
			OutFile << "|----" << biggest->_name << " = " << decstr(value) << endl;

			biggest = getAndRemoveBiggestRoutine(i, &value);

		}
			

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
 * after all instructions of the given program  */
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

    PIN_StartProgram();

    return 0;
}

