#ifndef _RTX_HEADER_
#define _RTX_HEADER_

#include <assert.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <setjmp.h>

#include "global_structs.h"
#include "kbcrt.h"
#include "debug.h"
#include "MsgEnvQueue.h"
#include "processQ.h"
#include "procPQ.h"
#include "global_defines.h"

void cleanup();
void die(int signal);

// User Processes
void processP();
void test_process();
void null_process();
void procA();
void procB();
void procC();
void clock_process();
void cci_process();

void displayClock(int);


// Trace Buffers
extern TraceBuffer SEND_TRACE_BUF;
extern TraceBuffer RECEIVE_TRACE_BUF;

extern pcb* CURRENT_PROCESS;
extern pcb* PREV_PROCESS;
extern MsgEnv* MSG_LIST[];
extern pcb* PCB_LIST[];
extern const InitProc INIT_TABLE[];

// global queues
extern MsgEnvQ* FREE_ENV_QUEUE;
extern proc_queue* BLOCKED_QUEUE;
extern MsgEnvQ* DISPLAYQ;
extern proc_pq* RDY_PROC_QUEUE;
extern MsgEnv * TIMEOUT_Q; // no need to allocate memory for this. It will just be assigned pointers to envelopes. Not a real queue.

// CCI envelope
extern MsgEnv* CCI_DISPLAY_ENV;

// Keyboard/CRT Unix Process Related
extern inputbuf * IN_MEM_P_KEY;	// pointer to structure that is the shared memory
extern outputbuf* IN_MEM_P_CRT;	// pointer to structure that is the shared memory
extern int IN_PID_KEYBOARD;		// pid of keyboard child process
extern int IN_PID_CRT;				// pid of crt child process
extern caddr_t MMAP_PTR_KEYBOARD;
extern caddr_t MMAP_PTR_CRT;
extern int BUFSIZE;
extern int FID_1;
extern int FID_2;
extern int FILE_STATUS;

extern float NUM_OF_TICKS;
extern int DISPLAY_CLOCK;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
extern const char * KB_SHARED_NAME;
extern const char * CRT_SHARED_NAME;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#endif

