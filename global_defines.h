#ifndef _GLOBAL_DEFINES_H
#define _GLOBAL_DEFINES_H

typedef int bool_t;
#define TRUE 1
#define FALSE 0
#define TRACE_LOG_SIZE 16

// Process IDs
#define PROCESS_COUNT 		   9 	// MUST MODIFY THIS IF YOU ADD A PROCESS
// This order MUST match the order of initialization in globals.c
#define KB_I_PROCESS_ID        0
#define CRT_I_PROCESS_ID       1
#define TIMER_I_PROCESS_ID	   2
#define PROCA_ID			   3
#define PROCB_ID			   4
#define PROCC_ID			   5
#define NULL_PROCESS_ID		   6
#define CLOCK_PROCESS_ID	   7
#define CCI_PROCESS_ID 		   8

// RTX Constants
#define MSG_ENV_SIZE 		   1000
#define MSG_ENV_COUNT 		   30
#define NUM_PRIORITY_LEVEL 	   4
#define STACK_SIZE             4*4096
#define STK_OFFSET             16

// error codes
#define SUCCESS 0
#define NULL_ARGUMENT -1
#define ILLEGAL_ARGUMENT -2
#define OTHER_ERROR -3

// atomic flags
#define ON TRUE
#define OFF FALSE

#define DEBUG 0

#endif
