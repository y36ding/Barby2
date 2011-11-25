#include "rtx.h"
#include "userAPI.h"

void procA ()
{
	MsgEnv *init_msg = (MsgEnv*)receive_message();

	while (init_msg==NULL) {
		init_msg = (MsgEnv*)receive_message();
	}
	release_message_env(init_msg);

	int num_count = 0;

	/*
	MsgEnv *env = (MsgEnv*)request_msg_env();
	ps("Timer for 5 secs");
	request_delay(5,WAKEUP10,env);
	env = (MsgEnv*)receive_message();
	while(env->msg_type!=WAKEUP10) {
		env = (MsgEnv*) receive_message();
	}
	*/

	//loop forever
	while(1)
	{
		//request a message envelope, and set the type to COUNT_REPORT
		//set the data field of the msg env equal to the counter
		MsgEnv *toB = (MsgEnv*)request_msg_env();
		while(toB == NULL) {
			toB = (MsgEnv*)request_msg_env();
		}
		toB->msg_type = COUNT_REPORT;
		sprintf(toB->data,"%d",num_count);
		//toB->time_delay=num_count;

		//send the message envelope to B
		//increment the counter and yield the processor to other processes
		send_message(PROCB_ID, toB);
		num_count++;
		usleep(50000);
		release_processor();
	}
}

void procB()
{
	//loop forever
	MsgEnv* msg_forward;
	while(1)
	{
		msg_forward = (MsgEnv*)receive_message();
#if DEBUG
		printf("Message Envelope data holds %d and %f\n",atoi(msg_forward->data),msg_forward->time_delay);//*(int*)
		//printf("Message Envelope data holds %i\n",msg_forward->time_delay);
#endif
		send_message(PROCC_ID, msg_forward);
		release_processor();
	}
}

void procC()
{
	//create local message queue to store msg envs in FIFO order
	MsgEnvQ *msgQueue = (MsgEnvQ*)MsgEnvQ_create();

	//initialize msg env pointers to keep track of messages
	ps("Proc C started!");
	MsgEnv *msg_env;
	while(1)
	{
		//while (MsgEnvQ_size(CURRENT_PROCESS->rcv_msg_queue) > 0 || MsgEnvQ_size(msgQueue) == 0)
		while(MsgEnvQ_size(msgQueue) == 0)
		{
			msg_env = (MsgEnv*)receive_message();
			MsgEnvQ_enqueue(msgQueue, msg_env);
		}

		msg_env = (MsgEnv*)MsgEnvQ_dequeue(msgQueue);

		if(msg_env->msg_type == COUNT_REPORT)
		{
			if( (atoi(msg_env->data)) % 20 == 0)
			//if( (msg_env->time_delay) % 20 == 0)
			{
				char tempData[20] = "\nProcess C\0";
				memcpy(msg_env->data,tempData,strlen(tempData)+1);
				send_console_chars(msg_env);

				//wait for output confirmation with 'DISPLAY_ACK' msg type
				while(1)
				{
					msg_env = (MsgEnv*)receive_message();
					if (msg_env->msg_type == DISPLAY_ACK)
					{
						printf("\nProcess C requesting 3 second delay!\n");
						if(request_delay(3, WAKEUP10, msg_env)!= SUCCESS)
						{
							printf("requesting delay for Process C went wrong");
						}
						while(1){
							msg_env = receive_message();
							if(msg_env->msg_type==WAKEUP10)
							{
								break;
							}else if(msg_env->msg_type == COUNT_REPORT){
								MsgEnvQ_enqueue(msgQueue, msg_env);
							}else{
								release_message_env(msg_env);
							}
						}
						break;
					}else if(msg_env->msg_type == COUNT_REPORT){
						//enqueue the msg env onto local msg queue if it originated from proc_A
						MsgEnvQ_enqueue(msgQueue, msg_env);
					}
					// random message. ignore
					else
						release_message_env(msg_env);
				}

			}
		}

		//deallocate message envelopes and release processor
        release_message_env(msg_env);

        if (MsgEnvQ_size(msgQueue) == 0) {
        	ps("Process C is releasing processor!");
        	release_processor();
        }
	}
}
