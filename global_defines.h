#ifndef _GLOBAL_DEFINES_H
#define _GLOBAL_DEFINES_H

typedef int bool_t;
#define TRUE 1
#define FALSE 0
#define TRACE_LOG_SIZE 16

// Process IDs
#define KB_I_PROCESS_ID        0
#define CRT_I_PROCESS_ID       1
#define P_PROCESS_ID 		   2
#define TIMER_I_PROCESS_ID	   3
#define PROCA_ID			   4
#define PROCB_ID			   5
#define PROCC_ID			   6
#define NULL_PROCESS_ID 7
#define TEST_PROCESS_ID 8

// RTX Constants
#define MSG_ENV_SIZE 500
#define MSG_ENV_COUNT 30
#define NUM_PRIORITY_LEVEL 4
#define STACK_SIZE             10*4096
#define STK_OFFSET            4
#define PROCESS_COUNT 8
#define NUM_PRIORITIES 4

// error codes
#define SUCCESS 0
#define NULL_ARGUMENT -1
#define ILLEGAL_ARGUMENT -2
#define OTHER_ERROR -3

// atomic flags
#define ON TRUE
#define OFF FALSE

#define DEBUG 1

#endif
