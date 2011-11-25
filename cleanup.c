#include "rtx.h"

//**************************************************************************
// routine to clean up things before terminating main program
// This stuff must be cleaned up or we have child processes and shared
//	memory hanging around after the main process terminates
void cleanup() {
	// terminate child process(es)
	kill(IN_PID_KEYBOARD, SIGINT);
	kill(IN_PID_CRT, SIGINT);

	// remove shared memory segment and do some standard error checks
	FILE_STATUS = munmap(MMAP_PTR_KEYBOARD, BUFSIZE);
	if (FILE_STATUS == -1) {
		printf("Bad munmap during cleanup\n");
	} else {
		printf("Unmapped shared memory for keyboard...\n");
	}

	FILE_STATUS = munmap(MMAP_PTR_CRT, BUFSIZE);
	if (FILE_STATUS == -1) {
		printf("Bad munmap during cleanup\n");
	} else {
		printf("Unmapped shared memory for CRT...\n");
	}

	// close the temporary mmap file
	FILE_STATUS = close(FID_1);
	if (FILE_STATUS == -1) {
		printf("Bad close of temporary mmap file during cleanup\n");
	} else {
		printf("Closed the file used for shared keyboard...\n");
	}

	FILE_STATUS = close(FID_2);
	if (FILE_STATUS == -1) {
		printf("Bad close of temporary mmap file during cleanup\n");
	} else {
		printf("Closed the file used for shared CRT...\n");
	}

	// unlink (i.e. delete) the temporary mmap file
	FILE_STATUS = unlink(KB_SHARED_NAME);
	if (FILE_STATUS == -1) {
		printf("Bad unlink during claeanup.\n");
	} else {
		printf("Deleted the file used for shared keyboard...\n");
	}

	FILE_STATUS = unlink(CRT_SHARED_NAME);
	if (FILE_STATUS == -1) {
		printf("Bad unlink during clean up.\n");
	} else {
		printf("Deleted the file used for shared CRT...\n");
	}

	// It is possible allocation failed in initialization
	// We try to ensure we don't try to free memory that was never allocated by always checking whether the pointer
	// is NULL or not
	int i;
	//ps("Freeing All Queues");
#if DEBUG
	printf("Destrying Free env queue, Address: %p\n",FREE_ENV_QUEUE);
#endif
	MsgEnvQ_destroy(FREE_ENV_QUEUE);
#if DEBUG
	printf("Destrying Blocked Queue, Address: %p\n",BLOCKED_QUEUE);
#endif
	proc_q_destroy(BLOCKED_QUEUE);
#if DEBUG
	printf("Destrying Display Q, Address: %p\n",DISPLAYQ);
#endif
	MsgEnvQ_destroy(DISPLAYQ);
#if DEBUG
	printf("Destroying Ready Proc Q, Address: %p\n",RDY_PROC_QUEUE);
#endif
	proc_pq_destroy(RDY_PROC_QUEUE);

	for (i = 0; i < PROCESS_COUNT; ++i) {
		// deallocate memory until we reach location where allocation may have failed
		if (PCB_LIST[i] == NULL)
			break;
		else {
			printf("Freeing pcb: %i, Name :%s, Pointer: %p, Rcv Queue:%p\n",i, PCB_LIST[i]->name, PCB_LIST[i], PCB_LIST[i]->rcv_msg_queue);
			MsgEnvQ_destroy(PCB_LIST[i]->rcv_msg_queue);
			free(PCB_LIST[i]);
		}
	}

	//printf("Freeing Messages and Their Data\n");
	for (i = 0; i < MSG_ENV_COUNT; ++i) {
		// deallocate memory until we reach location where allocation may have failed
		if (MSG_LIST[i] == NULL)
			break;
		else {
			//printf("Freeing envelope: %i, Address: %p\n",i,MSG_LIST[i]);
			if (MSG_LIST[i]->data == NULL) {
				free(MSG_LIST[i]);
				break;
			} else
				free(MSG_LIST[i]);
		}
	}
	exit(1);
}

// Die function
void die(int signal) {
	cleanup();
}
