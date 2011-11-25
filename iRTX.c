#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>

#include "rtx.h"
#include "userAPI.h"
#include "rtx_init.h"
#include "kernal.h"

//MERGE CONFLICT *****KEEP THE NULL AND TEST PROCESS*****

void null_process() {
	while(1) {
		release_processor();
		usleep(500000);
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

	//ps("in main step 1");
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
