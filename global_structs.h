#ifndef _GLOBAL_STRUCTS
#define _GLOBAL_STRUCTS

#include <setjmp.h>
#include "global_defines.h"

typedef void (*pc)();

char* msg_type(int i);
char* state_type(int i);

typedef enum msg_type {
    NONE, CONSOLE_INPUT, DISPLAY_ACK, COUNT_REPORT, WAKEUP10
}MsgType;

typedef struct MsgEnv {
   struct MsgEnv *next;
    int 	dest_pid;
    int 	sender_pid;
    MsgType 	msg_type;
    char    *data;
    int time_delay;
} MsgEnv;

typedef enum process_states {
    READY, EXECUTING, BLOCKED_ON_ENV_REQUEST, BLOCKED_ON_RCV, NEVER_BLK_RCV,
    INTERRUPTED
} ProcessState;

typedef struct msg_env_queue {
	MsgEnv* head;
	MsgEnv* tail;
}MsgEnvQ;

typedef struct process_control_block {
	int pid;
	int priority;
	ProcessState state;
	char* name;
	MsgEnvQ*  rcv_msg_queue;
	struct process_control_block* next;
	jmp_buf buf;
	pc location;
   	char * stack;
	int a_count;
	bool_t is_i_process;
} pcb;

typedef struct init_proc
{
	char* name;
	int pid;
	int priority;
	bool_t is_i_process;
	pc pc_location;
}InitProc;

typedef struct proc_queue {
    pcb *head;
    pcb *tail;
} proc_queue;

typedef struct proc_pq {
    int num_priorities;
    proc_queue **priority_queues;
} proc_pq;

typedef struct trace_log{
    int 	dest_pid;
    int 	sender_pid;
    MsgType 	msg_type;
    int time_stamp;
} TraceLog;

typedef struct circular_trace{
	int head;
	int count;
	TraceLog trace_log[TRACE_LOG_SIZE];
}TraceBuffer;

#endif
