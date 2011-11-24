#include "rtx.h"
#include "userAPI.h"

void cci_print(const char* print)
{
	sprintf(CCI_DISPLAY_ENV->data, "%s", print);
	int ret = send_console_chars(CCI_DISPLAY_ENV);
	if (ret==SUCCESS)
	{
        while (1)
        {
            MsgEnv *env = receive_message();
            if (env->msg_type == DISPLAY_ACK)
            {
                break;
            }
            else
            {
            	release_message_env(env);
            }
        }
	}
}

void cci_process()
{
	CCI_DISPLAY_ENV = request_msg_env();
	MsgEnv *cci_env, *kbd_input, *a_env, *process_status_env, *trace_env;
	kbd_input = request_msg_env(); // this wont be released since cci_env will eventually have this envelope.
	a_env = request_msg_env(); // this wont be released as process A will release it later
	process_status_env = request_msg_env();
	trace_env = request_msg_env();
	char formatted_msg[300];
	int retVal;

	while(1)
	{
		cci_env = NULL;

		const int twait = 500000;
		cci_print("CCI: ");
		get_console_chars(kbd_input);

		while(cci_env == NULL || cci_env->msg_type != CONSOLE_INPUT)
		{
					release_message_env(cci_env);
					usleep(twait);
					cci_env = receive_message();
		}

		//Obtained keyboard input
		char command [MAXCHAR];
		sprintf(command, "%s", cci_env->data);
		// Send a message to process A. This only happens once. If it has already been sent, then prompt user.
		if (strcmp(command, "s") == 0)
		{
			/*if (a_env != NULL)
			{
				int ret = send_message(PROCA_ID, a_env);
				if (ret != SUCCESS)
						cci_print("Failed to send message envlope and start A\n");
				a_env = NULL;
			}
			else
			{
				cci_print("A has already started.");
			}*/
			cci_print("Command 's' needs Chinmay's code\n");
		}
		else if(strcmp(command, "ps") == 0)
		{
			retVal = request_process_status(process_status_env);
			if (retVal != SUCCESS)
				cci_print("Failed to request process status");
			sprintf(formatted_msg, "%s", process_status_env->data);
			cci_print(formatted_msg);
		}
		else if(strcmp(command, "cd") == 0)
		{
			cci_print("We don't support this command yet");
		}
		else if(strcmp(command, "ct") == 0)
		{
			cci_print("We don't support this command yet");
		}
		else if(strcmp(command, "c") == 0)
		{
			cci_print("We don't support this command yet");
		}
		else if(strcmp(command, "c") == 0)
		{
			cci_print("We don't support this command yet");
		}
		else if(strcmp(command, "t") == 0)
		{
			release_message_env(cci_env);
			release_message_env(process_status_env);
			release_message_env(trace_env);
			release_message_env(CCI_DISPLAY_ENV);
			terminate();
		}
		// One space and enter results in CCI: being printed again
		else if(strcmp(command, " ") == 0)
		{
		}
		// If enter is directly pressed, display a different error
		else if(strcmp(command, "") == 0)
		{
			cci_print("Enter a command!\n");
		}
		// Default error message
		else
		{
			sprintf(formatted_msg, "The command %s is not supported\n", command);
			cci_print(formatted_msg);
		}
	}
}

