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

void test_process() {

	while (1)
	{
	ps("In Test Process");
	release_processor();
	ps("Back in Test Process");
	usleep(50000);
	}
}

void processP() {
	ps("ProcessP Started 1");
	// Test Trace Buffer
	int i;
	/*printf("Free queue size: %i\n", MsgEnvQ_size(FREE_ENV_QUEUE));
	for (i=0;i<70;++i)
	{
		MsgEnv* env = request_msg_env();
		sprintf(env->data, "Hello!");

		send_message(P_PROCESS_ID, env);
		MsgEnv* env2 = receive_message();
		printf("%s\n",env2->data);
		release_message_env(env2);
	}
	printf("Free queue size: %i\n", MsgEnvQ_size(FREE_ENV_QUEUE));*/

	/*MsgEnv* temp = request_msg_env();
	send_console_chars(temp);*/

	/*MsgEnv* env1 = request_msg_env();
	k_get_trace_buffer(env1);*/

	//sprintf(env1->data, ">>>>>>>>>>.Hello\n");
	//send_console_chars(env1);

	const int tWait = 500000;
	printf("Requesting env in Proc P\n");
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
	//k_process_switch(READY);
	//processP();

	ps("6");
	//MsgEnv *env = request_msg_env();
	//send_message(PROCA_ID,env);

	while (1)
		;

	// should never reach here, but in case we do, clean up after ourselves
	cleanup();
} // main
