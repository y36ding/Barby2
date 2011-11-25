#include "rtx.h"
#include "userAPI.h"
#include "cci.h"
#include "string.h"

MsgEnv* CCI_DISPLAY_ENV;

void cci_print(const char* print)
{
	sprintf(CCI_DISPLAY_ENV->data, "%s", print);
	int ret = send_console_chars(CCI_DISPLAY_ENV);
	if (ret==SUCCESS)
	{
            CCI_DISPLAY_ENV = receive_message();
	}
}

void cci_process()
{
	CCI_DISPLAY_ENV = request_msg_env();
	MsgEnv *cci_env = request_msg_env();
	MsgEnv*  a_env = request_msg_env(); // this wont be released as process A will release it later
	char formatted_msg[1000];
	int retVal;

	while(1)
	{
		const int twait = 500000;
		cci_print("CCI: ");
		//cci_env = request_msg_env();
		get_console_chars(cci_env);

		cci_env = receive_message();
		while(cci_env->msg_type != CONSOLE_INPUT)
		{
			release_message_env(cci_env);
			cci_env = receive_message();
		}

		//Obtained keyboard input
		char command [MAXCHAR];
		int offset = sprintf(command,  cci_env->data);
		// Check atleast 2 characters even if user entered 1.
		if (offset < 2)
			offset = 2;

		// Send a message to process A. This only happens once. If it has already been sent, then prompt user.
		if (strncasecmp(command, "s", offset) == 0)
		{
			if (a_env != NULL)
			{
				retVal = send_message(PROCA_ID, a_env);
				if (retVal != SUCCESS)
						cci_print("Failed to send message envlope and start A\n");
				a_env = NULL;
			}
			else
			{
				cci_print("A has already started.");
			}
		}
		else if(strncasecmp(command, "ps", offset ) == 0)
		{
			retVal = request_process_status(cci_env);
			if (retVal != SUCCESS)
				cci_print("Failed to request process status");
			sprintf(formatted_msg,  cci_env->data);
			cci_print(formatted_msg);
		}
		else if(strncasecmp(command, "cd", offset) == 0)
		{
			displayClock(1);
		}
		else if(strncasecmp(command, "ct", offset) == 0)
		{
			displayClock(0);
		}
		// Display Trace Buffers
		else if(strncasecmp(command, "b", offset) == 0)
		{
			retVal = get_trace_buffers(cci_env);
			if (retVal != SUCCESS)
				cci_print("Failed to get trace buffers");
			sprintf(formatted_msg, "%s", cci_env->data);
			cci_print(formatted_msg);
		}
		else if(strncasecmp(command, "c", 1) == 0)
		{
			if(command[4] != ':' || command[7] != ':' || strlen(command) != 10)
			{
				sprintf(formatted_msg, "Invalid format for command %s. It should be: c <hh>:<mm>:<ss>\n", "c");
				cci_print(formatted_msg);
			}
			else
			{
				const char * rawTimeString = command+2;
			    char hourString [3] = { '0', '0', '\0'};
			    char minString [3] = { '0', '0', '\0'};
			    char secString [3] = { '0', '0', '\0'};
			    int i, hr, min, sec;
			    for (i=0;i<2;i++)
			    {
			        hourString[i] =rawTimeString[i];
			        minString[i]=rawTimeString[3+i];
			        secString[i]=rawTimeString[6+i];
			    }

			    hour = atoi(hourString);
			    min = atoi(minString);
			    sec = atoi(secString);

			    if (hour>23 || min>59 || sec > 59)
			    {
			    	sprintf(formatted_msg, "Invalid input."
			    			" Ensure hh not greater than 23, mm not greater than 59, ss not greater than 59.\n");
			    	cci_print(formatted_msg);
			    }
			    else
			    	setClock(hour, min, sec);   // offset the "c " (c-space)
			}
		}
		else if(strncasecmp(command, "n", 1) == 0)
		{

			int priority, pid;
			// extract priority and pid from the command
			if (sscanf(command, "%*s %i %i", &priority, &pid)!=2)
			{
				sprintf(formatted_msg, "Invalid format for command %s. It should be: n <priority> <process id>\n", "n");
				cci_print(formatted_msg);
			}
			else
			{
				retVal = change_priority(priority, pid);
				sprintf(formatted_msg, "Priority: %i, Pid: %i\n", priority, pid);
				cci_print(formatted_msg);
				if (retVal == ILLEGAL_ARGUMENT)
				{
					sprintf(formatted_msg, "Invalid arguments. Ensure that the priority is between [0-3], and the process ID is a valid"
							"process ID other than the NULL process ID\n");
					cci_print(formatted_msg);
				}
				else if (retVal != SUCCESS)
				{
					cci_print("Priority of specified process could not be changed\n");
				}
			}
		}
		else if(strncasecmp(command, "t", offset) == 0)
		{
			// no need to release envelopes as terminate will free all memory from MSG_LIST
			terminate();
		}
		// debugging function. find where all the envelopes are.
		else if(strncasecmp(command, "en", offset) == 0)
		{
			int offset = sprintf(formatted_msg, "\nEnvelope Num\tHeld By\n");
			int i;
			for (i  = 0; i < MSG_ENV_COUNT; ++i)
			{
				const char* pcb_name;
				switch(MSG_LIST[i]->dest_pid)
				{
					case(-1):
						pcb_name = "NONE\0";
						break;
					default:
						pcb_name = PCB_LIST[MSG_LIST[i]->dest_pid]->name;
				}
				offset += sprintf(formatted_msg+offset, "%i\t\t%s\n", i+1, pcb_name);
			}
			cci_print(formatted_msg);
		}
		// One space and enter results in CCI: being printed again
		else if(strncasecmp(command, " ", offset) == 0)
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
		//printf("releasing %p\n", cci_env);
		//release_message_env(cci_env);
	}
	fflush(stdout);
	printf("=====================WTH! CCI came out of while loop!");
	fflush(stdout);
	terminate();
}

