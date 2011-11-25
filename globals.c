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
float NUM_OF_TICKS = 0;
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


//merge conflict here.... keep the NULL, TEST and CLOCK process or u will never get it to compile
// Initialization Table
const InitProc INIT_TABLE[PROCESS_COUNT] = {
		{ "Keyboard I proc\0", KB_I_PROCESS_ID,	0, TRUE, (pc)kbd_i_proc },
		{ "CRT I proc\0", CRT_I_PROCESS_ID, 0, TRUE,	(pc)crt_i_proc },
		{ "Timer I process\0", TIMER_I_PROCESS_ID, 0, TRUE, (pc)timer_i_proc },
		{ "Process A\0", PROCA_ID, 1, FALSE, (pc)procA },
		{ "Process B\0", PROCB_ID, 1, FALSE, (pc)procB },
		{ "Process C\0", PROCC_ID, 1, FALSE,(pc) procC },
		{ "NULL Process\0", NULL_PROCESS_ID, NUM_PRIORITY_LEVEL-1, FALSE,(pc) null_process }, // lowest priority
		{ "CLOCK PROCESS\0", CLOCK_PROCESS_ID, 0, FALSE,(pc) clock_process},
		{ "CCI Process\0", CCI_PROCESS_ID, 0, FALSE,(pc) cci_process}
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


