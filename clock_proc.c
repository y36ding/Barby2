#include "rtx.h"
#include "userAPI.h"



void clock_process() {

	while (1) {

		MsgEnv* env = (MsgEnv*)receive_message();

		if (!strcmp(env->data,"cd")) {

			//MsgEnv* temp = (MsgEnv*)request_msg_env();
			char time[10];
			sprintf(time,"%i:%i:%i",hh,mm,ss);
			send_console_chars(env);


		} else if (!strcmp(env->data,"ct")) {

		}


		release_processor();

	}

}
