#include <stdlib.h>
#include "rtx.h"
#include "rtx_init.h"
#include "iProcs.h"

int init_all_lists() {
	int i;
	int init_status = SUCCESS;
	jmp_buf kernel_buf;

	FREE_ENV_QUEUE = (MsgEnvQ*) MsgEnvQ_create();
	DISPLAYQ = (MsgEnvQ*) MsgEnvQ_create();
	BLOCKED_QUEUE = (proc_queue*) proc_q_create();
	RDY_PROC_QUEUE = (proc_pq*) proc_pq_create(NUM_PRIORITY_LEVEL);

	for (i = 0; i < PROCESS_COUNT; ++i)
	{
		PCB_LIST[i] = (pcb*) malloc(sizeof(pcb));
		if (PCB_LIST[i] == NULL) {
			init_status = OTHER_ERROR;
			break;
		}
		PCB_LIST[i]->pid = INIT_TABLE[i].pid;
		PCB_LIST[i]->priority = INIT_TABLE[i].priority;
		PCB_LIST[i]->name = INIT_TABLE[i].name;
		PCB_LIST[i]->rcv_msg_queue = (MsgEnvQ*) MsgEnvQ_create();
		PCB_LIST[i]->is_i_process = INIT_TABLE[i].is_i_process;
		PCB_LIST[i]->location = INIT_TABLE[i].pc_location;
		PCB_LIST[i]->stack = (char*)malloc(STACK_SIZE);
		PCB_LIST[i]->next = NULL;
		PCB_LIST[i]->a_count = 0;

		switch(PCB_LIST[i]->is_i_process)
		{
			case(1):
				PCB_LIST[i]->state = NO_STATE;
				break;
			default:
				PCB_LIST[i]->state = READY;
		}

		if (!(PCB_LIST[i]->is_i_process))
		{
			proc_pq_enqueue(RDY_PROC_QUEUE, PCB_LIST[i]);
			char* stack_location = PCB_LIST[i]->stack + STACK_SIZE - STK_OFFSET; // stack grows down
			// Initialize the stack and start pc
			if (setjmp(kernel_buf) == 0)
			{
				__asm__("movl %0, %%esp":"=m" (stack_location));
				if (setjmp(PCB_LIST[i]->buf) == 0)
				{
					longjmp(kernel_buf, 1);
				}
				else
				{
					//printf("Longjump worked, program run for first time\n");
					CURRENT_PROCESS->location();
					ps("I SHOULDN'T BE HERE!");
					pp(CURRENT_PROCESS);
				}
			} // set jmp
		} // is i process
	} // for loop

	for (i = 0; i < MSG_ENV_COUNT; i++)
	{
		MSG_LIST[i] = (MsgEnv*) malloc(sizeof(MsgEnv));
		if (MSG_LIST[i] == NULL) {
			init_status = OTHER_ERROR;
			break;
		}
		MSG_LIST[i]->time_delay = 0;
		MSG_LIST[i]->msg_type = NONE;
		MSG_LIST[i]->next = NULL;
		MSG_LIST[i]->data = (char*) malloc(sizeof(char) * MSG_ENV_SIZE);
		MSG_LIST[i]->dest_pid = -1;
		if (MSG_LIST[i]->data == NULL) {
			init_status = OTHER_ERROR;
			break;
		}
		MsgEnvQ_enqueue(FREE_ENV_QUEUE, MSG_LIST[i]);
	}

	ps("Done context!");
	return init_status;
}

void init_signals()
{
	sigset(SIGINT, die); // catch kill signals
	sigset(SIGBUS, die); // catch bus errors
	sigset(SIGHUP, die);
	sigset(SIGILL, die); // illegal instruction
	sigset(SIGQUIT, die);
	sigset(SIGABRT, die);
	sigset(SIGTERM, die);
	sigset(SIGSEGV, die); // catch segmentation faults

	sigset(SIGUSR1, kbd_i_proc);
	sigset(SIGUSR2, crt_i_proc);
	sigset(SIGALRM,timer_i_proc);

	ualarm((useconds_t) 100000, (useconds_t) 0);
}

void init_mmaps() {
	/* Create a new mmap file for read/write access with permissions restricted
	 to owner rwx access only */
	FID_1 = open(KB_SHARED_NAME, O_RDWR | O_CREAT | O_EXCL, (mode_t) 0755);
	if (FID_1 < 0) {
		printf("Bad Open of mmap file <%s>\n", KB_SHARED_NAME);
		exit(0);
	};

	FID_2 = open(CRT_SHARED_NAME, O_RDWR | O_CREAT | O_EXCL, (mode_t) 0755);
	if (FID_2 < 0) {
		printf("Bad Open of mmap file <%s>\n", CRT_SHARED_NAME);
		exit(0);
	};

	// make the file the same size as the buffer
	FILE_STATUS = ftruncate(FID_1, BUFSIZE);
	if (FILE_STATUS) {
		printf("Failed to ftruncate the file <%s>, FILE_STATUS = %d\n", KB_SHARED_NAME,
				FILE_STATUS);
		exit(0);
	}

	FILE_STATUS = ftruncate(FID_2, BUFSIZE);
	if (FILE_STATUS) {
		printf("Failed to ftruncate the file <%s>, FILE_STATUS = %d\n", CRT_SHARED_NAME,
				FILE_STATUS);
		exit(0);
	}

	// pass parent's process id and the file id to child
	char childarg1[20], childarg2[20]; // arguments to pass to child process(es)
	int mypid = getpid(); // get current process pid

	sprintf(childarg1, "%d", mypid); // convert to string to pass to child
	sprintf(childarg2, "%d", FID_1); // convert the file identifier


	// create the keyboard reader process
	// fork() creates a second process identical to the current process,
	// except that the "parent" process has in_pid = new process's ID,
	// while the new (child) process has in_pid = 0.
	// After fork(), we do execl() to start the actual child program.
	// (see the fork and execl man pages for more info)

	IN_PID_KEYBOARD = fork();
	if (IN_PID_KEYBOARD == 0) // is this the child process ?
	{
		execl("./keyboard", "keyboard", childarg1, childarg2, (char *) 0);
		// should never reach here
		fprintf(stderr, "kernal: can't exec keyboard, errno %d\n", errno);
		cleanup();
		exit(1);
	};

	// pass parent's process id and the file id to child CRT

	sprintf(childarg1, "%d", mypid); // convert to string to pass to child
	sprintf(childarg2, "%d", FID_2); // convert the file identifier

	IN_PID_CRT = fork();
	if (IN_PID_CRT == 0) // is this the child process ?
	{
		execl("./crt", "crt", childarg1, childarg2, (char *) 0);
		// should never reach here
		fprintf(stderr, "kernal: can't exec crt, errno %d\n", errno);
		cleanup();
		exit(1);
	};

	// the parent process continues executing here

	// sleep for a second to give the child process time to start
	sleep(1);

	// allocate a shared memory region using mmap
	// the child process also uses this region

	MMAP_PTR_KEYBOARD = (caddr_t)mmap((caddr_t) 0, /* Memory location, 0 lets O/S choose */
	BUFSIZE, /* How many bytes to mmap */
	PROT_READ | PROT_WRITE, /* Read and write permissions */
	MAP_SHARED, /* Accessible by another process */
	FID_1, /* the file associated with mmap */
	(off_t) 0); /* Offset within a page frame */
	if (MMAP_PTR_KEYBOARD == MAP_FAILED) {
		printf("Parent's memory map has failed for keyboard, about to quit!\n");
		die(0); // do cleanup and terminate
	};

	IN_MEM_P_KEY = (inputbuf *) MMAP_PTR_KEYBOARD; // pointer to shared memory
	// we can now use 'in_mem_p' as a standard C pointer to access
	// the created shared memory segment

	IN_MEM_P_KEY->ok_flag = 0;

	// allocate a shared memory region using mmap
	// the child process also uses this region

	MMAP_PTR_CRT = (caddr_t)mmap((caddr_t) 0, /* Memory location, 0 lets O/S choose */
	BUFSIZE, /* How many bytes to mmap */
	PROT_READ | PROT_WRITE, /* Read and write permissions */
	MAP_SHARED, /* Accessible by another process */
	FID_2, /* the file associated with mmap */
	(off_t) 0); /* Offset within a page frame */
	if (MMAP_PTR_CRT == MAP_FAILED) {
		printf("Parent's memory map has failed for CRT, about to quit!\n");
		die(0); // do cleanup and terminate
	};

	IN_MEM_P_CRT = (outputbuf *) MMAP_PTR_CRT; // pointer to shared memory
	// we can now use 'in_mem_p' as a standard C pointer to access
	// the created shared memory segment

	IN_MEM_P_CRT->ok_flag = 0;
}


