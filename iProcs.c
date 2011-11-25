#include "rtx.h"
#include "iProcs.h"
#include "kernal.h"

void crt_i_proc(int signum)
{
#if DEBUG
	//printf("Current process is: %s \n",CURRENT_PROCESS->name);
#endif
	int error = k_i_proc_interrupt(CRT_I_PROCESS_ID);

	if (error != SUCCESS)
	{
		printf("Error! Process Switch failed in CRT I process");
		cleanup();
		return;
	}

	ps("Inside CRT I proc");

	if (signum == SIGUSR2)
	{

#if DEBUG
			/*fflush(stdout);
			printf("Current PCB msgQ size is %i for process 1\n", MsgEnvQ_size(CURRENT_PROCESS->rcv_msg_queue) );
			ps("Got SIGUSR2");*/
#endif

			//ps("SIGUSR2 received!\n");
			MsgEnv* envTemp = NULL;
			envTemp = (MsgEnv*)MsgEnvQ_dequeue(DISPLAYQ);
			if (envTemp == NULL)
			{
				printf("Warning: Recieved a signal in CRT I process but there was no message.");
				return;
			}
			envTemp->msg_type = DISPLAY_ACK;
			k_send_message(envTemp->sender_pid, envTemp);
			//ps("CRT returning envelope!");
			//ps("Display ACK sent by crt");
			k_i_proc_return();
#if DEBUG
			//printf("Current process is: %s \n",CURRENT_PROCESS->name);
			//printf("Size of free queue: %i\n",MsgEnvQ_size(FREE_ENV_QUEUE));
#endif
			return;
	}

	MsgEnv* env = (MsgEnv*)k_receive_message();

	if (env==NULL) {
		env = (MsgEnv*)k_receive_message();
	}

	strcpy(IN_MEM_P_CRT->outdata,env->data);
#if DEBUG
		//printf("%s  %s\n",IN_MEM_P_CRT->outdata,env->data);
		//printf("The message data section holds \"%s\" \n",IN_MEM_P_CRT->outdata);
#endif

	MsgEnvQ_enqueue(DISPLAYQ,env);
	IN_MEM_P_CRT->ok_flag =  OKAY_DISPLAY;

	k_i_proc_return();
	return;
}

void kbd_i_proc(int signum)
{
	int error = k_i_proc_interrupt(KB_I_PROCESS_ID);
	if (error != SUCCESS)
	{
		printf("Error! Context Switch failed in keyboard I process");
		cleanup();
	}

	ps("Inside keyboard I proc");
	MsgEnv* env = (MsgEnv*)k_receive_message();

	if (env != NULL)
	{
		ps("Envelope recognized by kbd_i_proc");

		// Loop until writing in shared memory is done
		while (IN_MEM_P_KEY->ok_flag==OKAY_TO_WRITE);

		//copies into first parameter from second parameter of length+1 bytes
		if (IN_MEM_P_KEY->length != 0) {
			memcpy(env->data,IN_MEM_P_KEY->indata,IN_MEM_P_KEY->length + 1);
		} else {
			env->data[0] = '\0';
		}

		// Send message back to process that called us
		//merge conflict here.... keep my code
		/*if (!strcmp(IN_MEM_P_KEY->indata,"s")) {
			k_send_message(PROCA_ID,env);
		} else {*/
			env->msg_type=CONSOLE_INPUT;
			k_send_message(env->sender_pid ,env);
		//}

		ps("Keyboard sent message");

		IN_MEM_P_KEY->length = 0;
		IN_MEM_P_KEY->ok_flag = OKAY_TO_WRITE; // okay to write again
	}
	k_i_proc_return();
	return;
}

void timer_i_proc(int signum) {
	//printf("In get time. Num ticks %i\n", NUM_OF_TICKS);

	int error = k_i_proc_interrupt(TIMER_I_PROCESS_ID);
	if (error != SUCCESS) {
		printf("Error! Context Switch failed in keyboard I process");
		cleanup();
	}

	clock_inc_time();

	MsgEnv* msg_env = (MsgEnv*) k_receive_message();

	while (msg_env != NULL && msg_env->msg_type == WAKEUP10) {
		fflush(stdout);
		timeout_q_insert(msg_env);
		msg_env = (MsgEnv*) k_receive_message();
	}

	msg_env = (MsgEnv*) check_timeout_q();
	if (msg_env == NULL) {
		fflush(stdout);
	}

	if (msg_env != NULL) {
		// Send the envelope back
		fflush(stdout);
		msg_env->msg_type = WAKEUP10;

		//char tempData[50] = "Time Expired!\0";
		//memcpy(msg_env->data, tempData, strlen(tempData) + 1);
		k_send_message(msg_env->sender_pid, msg_env);
	}

	ualarm((useconds_t) 100000, (useconds_t) 0);
	k_i_proc_return();

}

int clock_get_time()
{
	//printf("In get time. Num ticks %i\n", NUM_OF_TICKS);
    return NUM_OF_TICKS;
}

void clock_inc_time() {
	//printf("In inc time. Num ticks %i\n", NUM_OF_TICKS);
    NUM_OF_TICKS++;
}

void clock_set_time(int time) {
	//printf("In set time. Num ticks %i\n", NUM_OF_TICKS);
    NUM_OF_TICKS = time;
}
