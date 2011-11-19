#include "rtx.h"
#include "userAPI.h"

void procA ()
{
	//receive the message envelope from CCI when user types in 's'
	//then deallocate the received message envelope
	ps("1");
	MsgEnv *init_msg = (MsgEnv*)receive_message();

	while (init_msg==NULL) {
		init_msg = (MsgEnv*)receive_message();
	}
	release_message_env(init_msg);
	ps("2");

	//initialize counter
	int num_count = 0;

	//loop forever
	while(1)
	{
		//request a message envelope, and set the type to COUNT_REPORT
		//set the data field of the msg env equal to the counter
		MsgEnv *toB = (MsgEnv*)request_msg_env();
		toB->msg_type = COUNT_REPORT;
		toB->data[0] = (char)num_count;
		toB->data[1] = '\0';

		//send the message envelope to B
		//increment the counter and yield the processor to other processes
		send_message(PROCB_ID, toB);
		num_count++;
		ps("test2");
		release_processor();
	}
}

void procB()
{
	//loop forever
	while(1)
	{
		//Receive a message envelope
		MsgEnv *msg_forward = (MsgEnv*)receive_message();

		//Send the message to proc_C
		send_message(PROCC_ID, msg_forward);
	}
}

void procC()
{
	//create local message queue to store msg envs in FIFO order
	MsgEnvQ *msgQueue = (MsgEnvQ*)MsgEnvQ_create();

	//initialize msg env pointers to keep track of messages
	MsgEnv *msg_env;
	MsgEnv *msg_env2;
	//loop forever
	while(1)
	{
		//if the local msg queue is empty, then receive a message, enqueue it to avoid null pointers for deallocation

		if(msgQueue->head == NULL)
		{
			msg_env = (MsgEnv*)receive_message();
			MsgEnvQ_enqueue(msgQueue, msg_env);
		}

		//dequeue the first msg on the local msg queue
		msg_env = (MsgEnv*)MsgEnvQ_dequeue(msgQueue);


		//check if the dequeued msg type is 'COUNT_REPORT'
		if(msg_env->msg_type == COUNT_REPORT)
		{
			//the msg originated from proc_A, check if the data is divisible by 20
			//********check the line below plz*********
			if( (*(int*)msg_env->data) % 20 == 0)
			{
				//get a message envelope, set the data to 'Process C'
				//send it to display on the screen
				msg_env2 = (MsgEnv*)request_msg_env();
				char tempData[20] = "Process C";
				memcpy(msg_env2->data,tempData,strlen(tempData));
				//msg_env2->data = '\nProcess C\n';
				send_console_chars(msg_env2);

				//wait for output confirmation with 'DISPLAY_ACK' msg type
				while(1)
				{
					//receive message, possibly from send_console_chars or proc_B
					msg_env2 = (MsgEnv*)receive_message();

					//check for msg type 'env2LAY_ACK' or 'COUNT_REPORT'
					if (msg_env2->msg_type == DISPLAY_ACK)
					{
						//go passive for 10seconds, use the msg env we got back from send_console_chars
						int stat = request_delay(10000, WAKEUP10, msg_env2);
						//if the delay request fails
						//*****what do i do here?***** print error
						if(stat != SUCCESS)
						{
							printf("\nWHAT?!?! something went wrong with the delay request...\n");
						}

						//resume after getting a 10seconds delay request
						while(1)
						{
							//reuse the msg_env2 pointer to get returned msg env from delay request
							msg_env2 = (MsgEnv*)receive_message();

							//check if its msg_type is 'WAKEUP_10' or 'COUNT_REPORT'
							if(msg_env2->msg_type == WAKEUP10)
							{
								//get out of "waiting for delay finish' loop
								break;
							}else if(msg_env2->msg_type == COUNT_REPORT){
								//enqueue the msg env onto local msg queue if it originated from proc_A
								MsgEnvQ_enqueue(msgQueue, msg_env2);
							}else{
								//shouldn't get here, useless msg env to proc_c
							}
						}

						//get out of 'waiting for output confirmation' loop
						break;
					}else if(msg_env2->msg_type == COUNT_REPORT){
						//enqueue the msg env onto local msg queue if it originated from proc_A
						MsgEnvQ_enqueue(msgQueue, msg_env2);
					}else{
						//shouldn't get here, useless msg env to proc_c
					}
				}
			}
		}

		//deallocate message envelopes and release processor
        release_message_env(msg_env);
        release_message_env(msg_env2);
        release_processor();
	}
}
