#include "rtx.h"
#include "iProcs.h"

// Trace Buffers
TraceBuffer SEND_TRACE_BUF;
TraceBuffer RECEIVE_TRACE_BUF;

// global variables
pcb* CURRENT_PROCESS = NULL;
pcb* PREV_PROCESS = NULL;
MsgEnvQ* FREE_ENV_QUEUE = NULL;
proc_queue* BLOCKED_QUEUE = NULL;
pcb* PCB_LIST[PROCESS_COUNT];
MsgEnvQ* DISPLAYQ;
MsgEnv* MSG_LIST[MSG_ENV_COUNT];
proc_pq* RDY_PROC_QUEUE = NULL;
MsgEnv * TIMEOUT_Q = NULL;

// Used in initialization
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const char * KB_SHARED_NAME = "keyboardBuffer"; //the name of the keyboard_memory file
const char * CRT_SHARED_NAME = "crtBuffer"; //the name of the crt_memory file
int NUM_OF_TICKS = 0;
int DISPLAY_CLOCK = 0;
int BUFSIZE = BUFFERSIZE;
inputbuf * IN_MEM_P_KEY = NULL;	// pointer to structure that is the shared memory
outputbuf* IN_MEM_P_CRT = NULL;	// pointer to structure that is the shared memory
int IN_PID_KEYBOARD;	// pid of keyboard child process
int IN_PID_CRT;			// pid of crt child process
caddr_t MMAP_PTR_KEYBOARD;
caddr_t MMAP_PTR_CRT;
int FID_1;
int FID_2;
int FILE_STATUS;


// Initialization Table
const InitProc INIT_TABLE[PROCESS_COUNT] = {
		{ "Keyboard I proc\0", KB_I_PROCESS_ID,	0, TRUE, (pc)kbd_i_proc },
		{ "CRT I proc\0", CRT_I_PROCESS_ID, 0, TRUE,	(pc)crt_i_proc },
		{ "P Process\0", P_PROCESS_ID, 0, FALSE,(pc) processP },
		{ "Timer I process\0", TIMER_I_PROCESS_ID, 0, TRUE, (pc)timer_i_proc },
		{ "Process A\0", PROCA_ID, 0, FALSE, (pc)procA },
		{ "process B\0", PROCB_ID, 2, FALSE, (pc)procB },
		{ "Process C\0", PROCC_ID, 2, FALSE,(pc) procC },
		{ "Test Process\0", TEST_PROCESS_ID, 0, FALSE,(pc) test_process },

};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


