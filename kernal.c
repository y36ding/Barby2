// kernal.c
#include "kernal.h"
#include "iProcs.h"
#include "processQ.h"
#include "procPQ.h"

pcb* pid_to_pcb(int pid)
{
	if (pid > PROCESS_COUNT || pid < 0)
	{
		ps("Invalid pid as parameter for pid_to_pcb!");
		return NULL;
	}
	return PCB_LIST[pid];
}

MsgEnv* k_request_msg_env()
{
	while (MsgEnvQ_size(FREE_ENV_QUEUE) == 0)
	{
		fflush(stdout);
		ps("One of Message Envelopes!");
		fflush(stdout);
		if(CURRENT_PROCESS->is_i_process)
			return NULL;
		proc_q_enqueue(BLOCKED_QUEUE, CURRENT_PROCESS);
		k_process_switch(BLOCKED_ON_ENV_REQUEST);
	}

	MsgEnv* free_env = (MsgEnv*)MsgEnvQ_dequeue(FREE_ENV_QUEUE);
	free_env->sender_pid = CURRENT_PROCESS->pid; // for debugging purposes in order to track envelopes
	return free_env;
}

int k_release_msg_env(MsgEnv* env)
{
	if (env == NULL)
		return NULL_ARGUMENT;
	int retVal;
	env->sender_pid = -1;
	retVal = MsgEnvQ_enqueue(FREE_ENV_QUEUE, env);
	// debugging purposes. -1 means no process has this envelope when we print envelopes using
	// cci
	if (retVal == SUCCESS)
	{
		if (proc_q_is_empty(BLOCKED_QUEUE) != TRUE)
		{
			pcb* blocked_process = proc_q_dequeue(BLOCKED_QUEUE);
			blocked_process->state = READY;
			retVal = proc_pq_enqueue(RDY_PROC_QUEUE, blocked_process);
		}
	}
	return retVal;
}

int k_send_message(int dest_process_id, MsgEnv *msg_envelope)
{
	//ps("In send message");
	pcb* dest_pcb =  pid_to_pcb(dest_process_id);

	if (!dest_pcb || !msg_envelope) {
		return NULL_ARGUMENT;
	}
	int retVal;
	msg_envelope->sender_pid = CURRENT_PROCESS->pid;
	msg_envelope->dest_pid = dest_process_id;
	retVal = MsgEnvQ_enqueue(dest_pcb->rcv_msg_queue, msg_envelope);

#if 1
	//printf("message SENT on enqueued on PID %i and its size is %i\n",dest_pcb->pid,MsgEnvQ_size(dest_pcb->rcv_msg_queue));
#endif

	if (retVal == SUCCESS)
	{
		if(dest_pcb->state == BLOCKED_ON_RCV)
		{
			dest_pcb->state = READY;
			retVal = proc_pq_enqueue(RDY_PROC_QUEUE,dest_pcb);
		}

#if DEBUG
			//printf("message SENT on enqueued on PID %i and its size is %i\n",dest_pcb->pid,MsgEnvQ_size(dest_pcb->rcv_msg_queue));
#endif

		k_log_event(&SEND_TRACE_BUF, msg_envelope);
	}
	return retVal;
}

MsgEnv* k_receive_message()
{
	while(MsgEnvQ_size(CURRENT_PROCESS->rcv_msg_queue) <= 0)
	{
		if (CURRENT_PROCESS->is_i_process == TRUE )
		{
			return NULL;
		}
		else
		{
#if DEBUG //leave my code in for merge conflict here
			printf("Process %s is getting blocked on receive\n",CURRENT_PROCESS->name);
#endif
			k_process_switch(BLOCKED_ON_RCV);
		}
	}

	// Message available
	MsgEnv *ret = (MsgEnv *)MsgEnvQ_dequeue(CURRENT_PROCESS->rcv_msg_queue);
	k_log_event(&RECEIVE_TRACE_BUF, ret);
	return ret;
}

int k_send_console_chars(MsgEnv *message_envelope)
{
	if (!message_envelope)
		return NULL_ARGUMENT;
	int retVal = k_send_message(CRT_I_PROCESS_ID, message_envelope);
	crt_i_proc(0);
	return retVal;
}

int k_get_console_chars(MsgEnv *message_envelope)
{
	if (!message_envelope)
		return NULL_ARGUMENT;
	int retVal = k_send_message( KB_I_PROCESS_ID, message_envelope);
	return retVal;
}

void atomic(bool_t state)
{
	return;
	static sigset_t oldmask;
	sigset_t newmask;
	if (state == TRUE)
	{
		CURRENT_PROCESS->a_count ++; //every time a primitive is called, increment by 1

		//mask the signals, so atomicity is enforced
		// if first time of atomic on
		if (CURRENT_PROCESS->a_count == 1)
		{
			sigemptyset(&newmask);
			sigaddset(&newmask, SIGALRM); //the alarm signal
			sigaddset(&newmask, SIGINT); // the CNTRL-C
			sigaddset(&newmask, SIGUSR1); // the KB signal
			sigaddset(&newmask, SIGUSR2); // the CRT signal
			sigprocmask(SIG_BLOCK, &newmask, &oldmask);
		}
	}
	else
	{
		//MERGEEE CONFLICT (LEAVE MY CODE, leave the comments in)
		ps("before decrementing atomic count, pcb state");
		//pp(CURRENT_PROCESS);
		CURRENT_PROCESS->a_count--; //every time a primitive finishes, decrement by 1
		//if all primitives completes, restore old mask, allow signals
		ps("after decrementing atomic count, check pcb state again");
		//pp(CURRENT_PROCESS);
		if (CURRENT_PROCESS->a_count == 0)
		{
			//restore old mask
			sigprocmask(SIG_SETMASK, &oldmask, NULL);
		}
	}
}

int k_i_proc_interrupt(int pid)
{
	pcb* p = (pcb*)pid_to_pcb(pid);
	if (p == NULL)
		return ILLEGAL_ARGUMENT;
	PREV_PROCESS = CURRENT_PROCESS;
	CURRENT_PROCESS = p;
	return SUCCESS;
}

void k_i_proc_return()
{
	pcb* temp = CURRENT_PROCESS;
	CURRENT_PROCESS = PREV_PROCESS;
	PREV_PROCESS = temp;
}

int k_request_delay(int delay, int wakeup_code, MsgEnv *msg_env)
{
#if DEBUG
    printf("%s is requesting a delay of %d with wakeup code %d\n", CURRENT_PROCESS->name,delay, wakeup_code);
#endif
    msg_env->msg_type = (MsgType)wakeup_code;
    msg_env->time_delay = delay;
    return k_send_message(TIMER_I_PROCESS_ID, msg_env);
}


void k_process_switch(ProcessState next_state)
{
	pcb* next_process = (pcb*)proc_pq_dequeue(RDY_PROC_QUEUE);
	if (next_process != NULL)
	{
		//printf("Inside Process Switch. Current Process is %s\n", CURRENT_PROCESS->name);
		//ps("Inside Process Switch. Current process is:");
		//pp(CURRENT_PROCESS);

		CURRENT_PROCESS->state = next_state;
		pcb* old_process = CURRENT_PROCESS;
		CURRENT_PROCESS = next_process;
		CURRENT_PROCESS->state = EXECUTING;

		//printf("Next Process is %s\n", next_process->name);
		//ps("Next process process is:");
		//pp(next_process);

		k_context_switch(old_process->buf, CURRENT_PROCESS->buf);
	}
	//ps("Back in process switch after context");
}

void k_context_switch(jmp_buf prev, jmp_buf next)
{
	int val = setjmp(prev);
	if (val == 0)
	{
		longjmp(next, 1);
	}
}

int k_release_processor()
{
	int retVal = proc_pq_enqueue(RDY_PROC_QUEUE, CURRENT_PROCESS);
	if (retVal == SUCCESS)
	{
		k_process_switch(READY);
	}
	return retVal;
}

int k_request_process_status(MsgEnv *env)
{
    int offset = sprintf(env->data, "\nProcess Name\t\tProcess ID\tPriority\tState\n");
	int i;
	for (i = 0; i < PROCESS_COUNT; ++i)
	{
		offset += sprintf(env->data+offset, "%s\t\t%i\t\t%i\t\t%s\n", PCB_LIST[i]->name, PCB_LIST[i]->pid, PCB_LIST[i]->priority, state_type(PCB_LIST[i]->state));
	}
	sprintf(env->data+offset, "\n");
	return SUCCESS;
}

int k_terminate()
{
	cleanup();
	return SUCCESS;
}

int k_change_priority(int target_priority, int target_pid)
{
	if (target_priority > NUM_PRIORITY_LEVEL-1 || target_priority < 0)
			return ILLEGAL_ARGUMENT;

	pcb* target_pcb = pid_to_pcb(target_pid);
	if (target_pcb->pid == NULL_PROCESS_ID)
		return ILLEGAL_ARGUMENT;

	// if on a ready queue, take if off, change priority, and put it back on
    if(target_pcb->state == READY)
    {
        proc_pq_remove(RDY_PROC_QUEUE, target_pcb);
        target_pcb->priority = target_priority;
        proc_pq_enqueue(RDY_PROC_QUEUE, target_pcb);
    }
    else
    {
    	target_pcb->priority = target_priority;
    }
    return SUCCESS;
}

int get_trace_tail(TraceBuffer* trace_buff)
{
	return (trace_buff->head + trace_buff->count)%TRACE_LOG_SIZE;
}

//env trace helper function for k_send and k_recieve to log events
int k_log_event(TraceBuffer* trace_buf, MsgEnv *env)
{
	if (env == NULL || trace_buf == NULL)
		return NULL_ARGUMENT;

	int tail = get_trace_tail(trace_buf);
	trace_buf->trace_log[tail].dest_pid = env->dest_pid;
	trace_buf->trace_log[tail].sender_pid = env->sender_pid;
	trace_buf->trace_log[tail].msg_type = env->msg_type;
	trace_buf->trace_log[tail].time_stamp = clock_get_time();
	if (trace_buf->count == TRACE_LOG_SIZE)
		trace_buf->head = (trace_buf->head + 1)%TRACE_LOG_SIZE;
	else
		trace_buf->count++;
	return SUCCESS;
}

int k_get_trace_buffer( MsgEnv *msg_env )
{
	if (msg_env == NULL)
		return 1;

    int send_tail = get_trace_tail(&SEND_TRACE_BUF);
    int receive_tail = get_trace_tail(&RECEIVE_TRACE_BUF);
    int i;
    int offset = sprintf(msg_env->data, "\nSend Trace Buffer\nTrace Num\tDest Pid\tSender Pid\tMessage Type\tTime Stamp\n");

    i = SEND_TRACE_BUF.head;
    int count = 1;
    do
    {
    	TraceLog* log = &SEND_TRACE_BUF.trace_log[i];
    	offset += sprintf(msg_env->data+offset, "%i\t\t%i\t\t%i\t\t%s\t%i\n", count, log->dest_pid, log->sender_pid, msg_type(log->msg_type), log->time_stamp);
    	i = (i+1)%TRACE_LOG_SIZE;
    	count++;
    }while(i!=send_tail);

   offset += sprintf(msg_env->data+offset, "\nReceive Trace Buffer\nTrace Num\tDest Pid\tSender Pid\tMessage Type\tTime Stamp\n");
    i =  RECEIVE_TRACE_BUF.head;
    count = 1;
    do
    {
    	TraceLog* log = &RECEIVE_TRACE_BUF.trace_log[i];
    	offset += sprintf(msg_env->data+offset, "%i\t\t%i\t\t%i\t\t%s\t%i\n", count, log->dest_pid, log->sender_pid, msg_type(log->msg_type), log->time_stamp);
    	i = (i+1)%TRACE_LOG_SIZE;
    	count++;
    }while(i!= receive_tail);
    sprintf(msg_env->data+offset, "\n");
    return SUCCESS;
}
