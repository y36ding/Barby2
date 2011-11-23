#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

#include "rtx.h"
#include "userAPI.h"
#include "rtx_init.h"
#include "kernal.h"

void null_process() {

	while (1)
	{
	ps("In Null Process");
	release_processor();
	ps("Back in Null Process");
	usleep(500000);
	}
}

void processP() {
	ps("ProcessP Started 1");
	// Test Trace Buffer
	/*int i;
	for (i=0;i<20;++i)
	{
		MsgEnv* env = request_msg_env();
		send_message(P_PROCESS_ID, env);
		MsgEnv* env2 = receive_message();
		release_message_env(env2);
	}
	MsgEnv* env1 = request_msg_env();
	k_get_trace_buffer(env1);*/
	MsgEnv* env2 = request_msg_env();
	release_processor();
	ps("Back in process P 2");
	MsgEnv* env3 = request_msg_env();
	env2->dest_pid = 5;
	release_processor();
	ps("Back in process P again 3");
	printf("the dest id in env2 is: %i\n", env2->dest_pid);
	release_processor();
	ps("Back in process P once more 4");
	printf("the dest id in env2 is: %i\n", env2->dest_pid);
	const int tWait = 500000;

	ps("Requesting env in Proc P");
	MsgEnv* env = request_msg_env();
	ps("Envelopes Allocated");

	while (1) {
	 ps("Asking for Characters");

	 // Request keyboard input
	 get_console_chars(env);

	 ps("Back in Process P. waiting for Keyboard msg");
	 // Check if keyboard i proc sent a confirmation message
	 env = receive_message();
	 while (env == NULL) {
	 usleep(tWait);
	 env = (MsgEnv*) receive_message();
	 if (env != NULL && env->msg_type == CONSOLE_INPUT) {
	 #if DEBUG
		 printf("Keyboard Input Acknowledged");
	 #endif
	 }
	 }

	 // Send the input to CRT
	 send_console_chars(env);

	 // Check if CRT displayed
	 env = receive_message();
	 while (env == NULL) {
	 usleep(tWait);

	 env = receive_message();
	 if (env != NULL && env->msg_type == DISPLAY_ACK) {
		 release_message_env(env);
	 #if DEBUG
	 	 printf("CRT Display Acknowledged");
	 #endif
	 }
	 }
	 release_message_env(env);
	 }
}

//**************************************************************************
int main() {

	if (init_all_lists() != SUCCESS) {
		printf(
				"Failed to initialize the process and message envelope list. Exiting the OS.\n");
		cleanup();
	}
	init_signals();
	init_mmaps() ;


	/*MsgEnv* timer_env = request_msg_env();
	 k_request_delay(3,WAKEUP10,timer_env);
	 MsgEnv* timer_env2 = request_msg_env();
	 k_request_delay(6,WAKEUP10,timer_env2);*/

	//processP();
	//ps("PROC A");

	ps("in main step 1");
	//pp(CURRENT_PROCESS);
	//k_process_switch(READY);

	//pstacks();
	// Enter scheduler
    pcb * first_pcb = proc_pq_dequeue(RDY_PROC_QUEUE);
    CURRENT_PROCESS = first_pcb;
    first_pcb->state = EXECUTING;
    longjmp(first_pcb->buf, 1);

	ps("Back in main");
	ps("6");
	//MsgEnv *env = request_msg_env();
	//send_message(PROCA_ID,env);

	while (1)
		;

	// should never reach here, but in case we do, clean up after ourselves
	cleanup();
} // main
