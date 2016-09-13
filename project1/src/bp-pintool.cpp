/*
 * A new PINTOOL which simulates two branch predictor models. It provides 1-bit and 2-bit models.
 *
 * Author: Gustavo CIOTTO PINTON
 * Computer Architecture II MO601B
 */

#include <iostream>
#include <fstream>
#include <string.h>
#include "pin.H"

/* Hash function is the least significant bits of PC. HASH_FUNCTION defines its number and the
   the array sizes. For example, if it is 4096, 10 bits are used. */
#define HASH_FUNCTION 4096

/* Abstract class for a branch predictor model. */
class GenericBP {

public:

	GenericBP ();

	BOOL prediction[HASH_FUNCTION];
	ADDRINT target[HASH_FUNCTION];
   	BOOL getPrediction (ADDRINT pc);
	ADDRINT getPredictedTarget (ADDRINT pc);
	/* All subclasses must define update method. */
	virtual VOID update (ADDRINT pc, BOOL taken, ADDRINT target) 	= 0;

};

GenericBP::GenericBP (){
	
	/* Initializes all arrays. */
	for (UINT32 i = 0; i < HASH_FUNCTION; i++) {
		target[i] = 0;
		prediction[i]	 = false;
	}

}

ADDRINT GenericBP::getPredictedTarget (ADDRINT pc) {
	return target [pc & (HASH_FUNCTION - 1) ];
}

BOOL GenericBP::getPrediction (ADDRINT pc) {
	return prediction [pc & (HASH_FUNCTION - 1) ];
}


/* 1-bit branch predictor model. */
class BP1bit : public GenericBP {

public:
	
	VOID update (ADDRINT pc, BOOL taken, ADDRINT target);

};

/* the branch predictor is updated only based on last ocurrence */
VOID BP1bit::update (ADDRINT pc, BOOL taken, ADDRINT t) {
	if (taken) {
		target [pc & (HASH_FUNCTION - 1) ] = t;
	}

	prediction [pc & (HASH_FUNCTION - 1) ] = taken;
}

/* A possible state of the 2-bit branch predictor */
enum State {
	STRONG_TAKEN,
	WEAK_TAKEN,
	WEAK_NOT_TAKEN,
	STRONG_NOT_TAKEN
};

/* 2-bit branch predictor model */
class BP2bit : public GenericBP {

public:
	
	enum State bpState [HASH_FUNCTION];
	BP2bit ();
	VOID update (ADDRINT pc, BOOL taken, ADDRINT target);

};

/* Updates the state according to we discussed in class. */
VOID BP2bit::update (ADDRINT pc, BOOL taken, ADDRINT t) {

	enum State aState = bpState[pc & (HASH_FUNCTION - 1)];
	UINT32 index = pc & (HASH_FUNCTION - 1);

	if (aState == WEAK_NOT_TAKEN) {

		if (!taken) 
			bpState[index] = STRONG_NOT_TAKEN;

		else {

			bpState[index] = WEAK_TAKEN;
			target [index] = t;
		}	

		prediction [index] = taken;

		return;
	}

	if (aState == WEAK_TAKEN) {

		if (!taken) 
			bpState[index] = WEAK_NOT_TAKEN;

		else {
			bpState[index] = STRONG_TAKEN;
			target [index] = t;
		}	

		prediction [index] = taken;

		return;
	}

	if (aState == STRONG_NOT_TAKEN) {

		if (taken)
			bpState[index] = WEAK_NOT_TAKEN;

		prediction [index] = false;

		return;
	}

	if (aState == STRONG_TAKEN) {

		if (!taken)
			bpState[index] = WEAK_TAKEN;

		prediction [index] = true;
		target [index] = t;
	}


}

BP2bit::BP2bit() {

	for (UINT32 i = 0; i < HASH_FUNCTION; i++)
		bpState[i] = WEAK_NOT_TAKEN;	
}

/* Predictor. Change according to preference. */
BP1bit* BP = new BP1bit();
/* Performance counters */
UINT64 missdirectionpredicted = 0, misstargetpredicted = 0, misspredicted = 0, wellpredicted = 0;

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "most-used.out", "specify output file name");

/* Output file handler */
ofstream OutFile;


/* Analysis routine. Called before all branch or call instruction. */
VOID ProcessBranch(ADDRINT pc, ADDRINT target, bool taken) { 
	BOOL pred = BP->getPrediction(pc); 
	ADDRINT predTarget = BP->getPredictedTarget(pc);
	BOOL ok = true;

	if( pred != taken ) { 
		missdirectionpredicted++;
		ok = false;
	} 

	if( predTarget != target ) { 
		misstargetpredicted++;
		ok = false;
	}

	if (ok) 
		wellpredicted++;
	else misspredicted++;

	BP->update(pc, taken, target); 
} 


/* Instrumentation routine. */
VOID Instruction(INS ins, VOID *v)  
{ 
  if( INS_IsDirectBranchOrCall(ins) ) 
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) ProcessBranch, 
					IARG_ADDRINT, INS_Address(ins), 
					IARG_ADDRINT, INS_DirectBranchOrCallTargetAddress(ins), 
					IARG_BRANCH_TAKEN, IARG_END); 
} 


/* Callback function called when application is exiting */
VOID endTool(INT32 code, VOID *v)
{
    /* Write to a file since cout and cerr maybe closed by the application */
    OutFile << "Miss predicted direction = " << decstr(missdirectionpredicted) << endl;
    OutFile << "Miss predicted target = " << decstr(misstargetpredicted) << endl;
    OutFile << "Miss predicted = " << decstr(misspredicted) << endl;
    OutFile << "Well predicted = " << decstr(wellpredicted) << endl;

    OutFile.close();
}

/* Default help message */
INT32 Usage()
{
    PIN_ERROR( "This Pintool prints a trace of memory addresses\n"
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

int main(int argc, char *argv[]) { 

    if (PIN_Init(argc, argv)) return Usage();

    OutFile.open(KnobOutputFile.Value().c_str(), ofstream::out | ofstream::app);

	INS_AddInstrumentFunction(Instruction, 0); 
    PIN_AddFiniFunction(endTool, 0);
	PIN_StartProgram(); 
}
