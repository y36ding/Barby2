#include "rtx.h"
#include "userAPI.h"
#include "cci.h"

int hour, min, sec;
int clockDisplayRequest; //0 when not displaying, 1 when displaying
MsgEnvQ *envQ;
MsgEnv *generalEnv, *timeoutEnv, *displayEnv;
int checkBit, clockTime;

void clock_process() {

	envQ = MsgEnvQ_create();
	clockDisplayRequest = 0;
	clockTime = 0;
	timeoutEnv = (MsgEnv*) request_msg_env();
	displayEnv = (MsgEnv*) request_msg_env();

	checkBit = request_delay(1,WAKEUP10,timeoutEnv);
	if (checkBit!=SUCCESS) {
		ps("Message couldnt be sent to timer iproc from clock process");
	}


	while (1) {
		if (MsgEnvQ_is_empty(envQ))
			generalEnv = receive_message();
		else
			generalEnv = MsgEnvQ_dequeue(envQ);


		if (generalEnv->msg_type==DISPLAY_ACK) {
			displayEnv = generalEnv;
			continue;
		}

		//envelope from timing services
		if (generalEnv->msg_type == WAKEUP10) {
			//release_message_env(generalEnv);
			timeoutEnv = generalEnv;
			checkBit = request_delay(1, WAKEUP10, timeoutEnv);
			if (checkBit != SUCCESS) {
				ps("Message couldnt be sent to timer iproc from clock process");
			}
			//86400 = 24hrs in secs
			clockTime++;//(int32_t)((clock_get_system_time()-ref)/10+offset)%SEC_IN_HR;
			if (clockDisplayRequest) {
				/*int hours = clockTime%3600;
				int mins = (clockTime - hours*60*60)%60;
				int secs = (clockTime - hours*60*60 - mins*60)%60;*/
				int hours = clockTime/3600;
				int mins = (clockTime%3600)/60;
				int secs = clockTime%60; // whatever is remainder is seconds

				sprintf(displayEnv->data,"\n%02i:%02i:%02i",hours,mins,secs);
				send_console_chars(displayEnv);
			}
		}
	}
	release_processor();
}


void setClock (hour, min, sec)
{
    clockTime = (hour*3600 + min*60 + sec);
}

//setter function for wallClock display status bit
void displayClock (int newStatus)
{
	if (newStatus==0 || newStatus==1)
	{
		clockDisplayRequest = newStatus;
	}
}
