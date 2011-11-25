#ifndef _KB_CRT_HEADER_
#define _KB_CRT_HEADER_

#include "global_defines.h"

// structures for shared memory
#define MAXCHAR MSG_ENV_SIZE

#define BUFFERSIZE MAXCHAR+20

// CRT/KB Flags
#define OKAY_TO_WRITE 0
#define DONE_WRITING 1
#define WAITING_TO_BE_WRITTEN 0
#define OKAY_DISPLAY 1

typedef struct {
	int ok_flag;
	char indata[MAXCHAR];
	int length;
} inputbuf;

typedef struct {
	int ok_flag;
	char outdata[MAXCHAR];
	int length;
} outputbuf;

#endif
