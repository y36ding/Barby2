#include "debug.h"
#include "rtx.h"

// This was not included in debug.c since we require access to "rtx.h". But "rtx.h" includes debug.h so its circular inclusion

void pstacks()
{
#if DEBUG
	int i;
	printf("Free Env Queue: %p\n", FREE_ENV_QUEUE);
	printf("Blocked Queue: %p\n", BLOCKED_QUEUE);
	printf("Display Q: %p\n", DISPLAYQ);
	printf("Ready Proc Q: %p\n", RDY_PROC_QUEUE);
	for (i=0;i<PROCESS_COUNT;++i)
	{
		printf("PCB: %i, Pointer: %p\n",i, PCB_LIST[i]);
	}
	for(i=0;i<MSG_ENV_COUNT;++i)
	{
		printf("Envelope Number: %i, Pointer: %p\n",i, MSG_LIST[i]);
	}
#endif
}
