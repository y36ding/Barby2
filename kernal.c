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
	// the real code will keep on trying to search free env
	// queue for envelope and get blocked otherwise
	if (MsgEnvQ_size(FREE_ENV_QUEUE) == 0) {
		//ps("NO FREE ENVELOPES");
		return NULL;
	}

	MsgEnv* free_env = (MsgEnv*)MsgEnvQ_dequeue(FREE_ENV_QUEUE);
	return free_env;
}

int k_release_message_env(MsgEnv* env)
{
	if (env == NULL)
		return NULL_ARGUMENT;

	//env->next=NULL;
	//env->data='\0';
	//env->dest_pid=0;
	//env->time_delay=0;
	//env->msg_type=NONE;
	//env->sender_pid=0;

	MsgEnvQ_enqueue(FREE_ENV_QUEUE, env);

#if DEBUG
	//printf("MSG ENV RELEASED!\n");
#endif
	// check processes blocked for allocate envelope later
	return SUCCESS;
}

int k_send_message(int dest_process_id, MsgEnv *msg_envelope)
{

		ps("In send message");


	/*
	pcb* dest_pcb =  pid_to_pcb(dest_process_id);

	if (!dest_pcb || !msg_envelope) {
		return NULL_ARGUMENT;
	}

	msg_envelope->sender_pid = CURRENT_PROCESS->pid;
	msg_envelope->dest_pid = dest_process_id;

	if (DEBUG==1) {
		fflush(stdout);
		printf("Dest pid is %i\n",dest_pcb->pid);
		fflush(stdout);
	}

	MsgEnvQ_enqueue(dest_pcb->rcv_msg_queue, msg_envelope);
	if (DEBUG==1){
		printf("message SENT on enqueued on PID %i and its size is %i\n",dest_pcb->pid,MsgEnvQ_size(dest_pcb->rcv_msg_queue));
	}
	k_log_event(&SEND_TRACE_BUF, msg_envelope);
	return SUCCESS;*/

	//full implementation version
	pcb* dest_pcb =  pid_to_pcb(dest_process_id);
	if (!dest_pcb || !msg_envelope) {
		return NULL_ARGUMENT;
	}
	msg_envelope->sender_pid = CURRENT_PROCESS->pid;
	msg_envelope->dest_pid = dest_process_id;
	MsgEnvQ_enqueue(dest_pcb->rcv_msg_queue, msg_envelope);
#if DEBUG
	//printf("message SENT on enqueued on PID %i and its size is %i\n",dest_pcb->pid,MsgEnvQ_size(dest_pcb->rcv_msg_queue));
#endif

	if(dest_pcb->state == BLOCKED_ON_RCV)
	{
		dest_pcb->state = READY;
		proc_pq_enqueue(RDY_PROC_QUEUE,dest_pcb);
	}
	if (DEBUG==1){
		printf("message SENT on enqueued on PID %i and its size is %i\n",dest_pcb->pid,MsgEnvQ_size(dest_pcb->rcv_msg_queue));
	}
	k_log_event(&SEND_TRACE_BUF, msg_envelope);
	return SUCCESS;
}

MsgEnv* k_receive_message()
{
	//printf("===CURRENT PROCESS = %i\n",CURRENT_PROCESS->pid);
	while(MsgEnvQ_size(CURRENT_PROCESS->rcv_msg_queue) <= 0)
	{
		if (CURRENT_PROCESS->is_i_process == TRUE ){
			return NULL;
		}else{
#if DEBUG //leave my code in for merge conflict here
			printf("Process %s is getting blocked on receive\n",CURRENT_PROCESS->name);
#endif
			k_process_switch(BLOCKED_ON_RCV);
		}
	}

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

int k_pseudo_process_switch(int pid)
{
	pcb* p = (pcb*)pid_to_pcb(pid);
	if (p == NULL)
		return ILLEGAL_ARGUMENT;
	PREV_PROCESS = CURRENT_PROCESS;
	CURRENT_PROCESS = p;
	return SUCCESS;
}

void k_return_from_switch()
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
	//pp(next_process);
	// Note this is not checking for null process. It is just for checking the dequeue
	// was successful
	if (next_process != NULL)
	{
		//ps("Inside Process Switch. Current process is:");
		//pp(CURRENT_PROCESS);

		CURRENT_PROCESS->state = next_state;
		pcb* old_process = CURRENT_PROCESS;
		CURRENT_PROCESS = next_process;
		CURRENT_PROCESS->state = EXECUTING;
		k_context_switch((old_process->buf), (CURRENT_PROCESS->buf));
	}
	//ps("Back in process switch after context");
}

void k_context_switch(jmp_buf prev, jmp_buf next)
{
	int val = setjmp(prev);
	//ps("got here");
	if (val == 0)
	{
		//ps("got here2");
		longjmp(next, 1);
	}
	//pp(CURRENT_PROCESS);
	//printf("%p",CURRENT_PROCESS->buf);
	//ps("Back in context switch after longjump");
}

int k_release_processor()
{
	//pp(CURRENT_PROCESS);
	proc_pq_enqueue(RDY_PROC_QUEUE,CURRENT_PROCESS);
	k_process_switch(READY);
	//ps("in k release processor before returning");
	return SUCCESS;
}

int k_request_process_status(MsgEnv *env)
{
	char status[500];
    char header[80];
    sprintf(header, "Process ID\tPriority\tState\n");
    strcat(status, header);
	int i;
	for (i = 0; i < PROCESS_COUNT; ++i)
	{
		char proc_status[100];
		sprintf(proc_status, "%i\t\t%i\t\t%s\n", PCB_LIST[i]->pid, PCB_LIST[i]->priority, state_type(PCB_LIST[i]->state));
		strcat(status, proc_status);
		/**status = PCB_LIST[i]->pid;
		status ++;
		*status = PCB_LIST[i]->state;
		status++;
		*status = PCB_LIST[i]->priority;
		status++;*/
	}
	sprintf(env->data, status);
	return SUCCESS;
}

int k_terminate()
{
	cleanup();
	return SUCCESS;
}

int k_change_priority(int target_priority, int target_pid)
{
	if (target_priority >= 3 || target_priority < 0)
			return ILLEGAL_ARGUMENT;

	pcb* target_pcb = pid_to_pcb(target_pid);
	if (target_pcb->pid == NULL_PROCESS_ID)
		return ILLEGAL_ARGUMENT;

	// if on a ready queue, take if off, change priority, and put it back on
    if(target_pcb->state == READY)
    {
        proc_pq_dequeue(RDY_PROC_QUEUE);
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
	trace_buf->trace_log[tail].time_stamp = 4; // Should this be a RTX function?
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
    int send_size = SEND_TRACE_BUF.count;
    int receive_size = RECEIVE_TRACE_BUF.count;
    int i;

    // Assign the memory locations which will be written to in the message envelope
   /* int* buff_size = (int*)msg_env->data;
    *buff_size = send_size;
    buff_size++;
    *buff_size = receive_size;
    buff_size++;*/

    char send_header[80];
    sprintf(send_header, "Send Trace Buffer\nTrace Num\tDest Pid\tSender Pid\tMessage Type\tTime Stamp\n");
    strcat(msg_env->data, send_header);
    TraceLog* log_stack =  (TraceLog*)msg_env->data;
    i = SEND_TRACE_BUF.head;
    int count = 1;
   do
    {
    	TraceLog* log = &SEND_TRACE_BUF.trace_log[i];
    	char trace[100];

    	sprintf(trace, "%i\t%i\t%i\t%s\t%i\n", count, log->dest_pid, log->sender_pid, msg_type(log->msg_type), log->time_stamp);
    	strcat(msg_env->data, trace);
    	break;
    	/*log_stack->dest_pid = log->dest_pid;
    	log_stack->msg_type = log->msg_type;
    	log_stack->sender_pid = log->sender_pid;
    	log_stack->time_stamp = log->time_stamp;
    	log_stack++;*/
    	i = (i+1)%TRACE_LOG_SIZE;
    }while(i!=send_tail);

   char receive_header[80];
   sprintf(receive_header, "Receive Trace Buffer\nTrace Num\tDest Pid\tSender Pid\tMessage Type\tTime Stamp\n");
   strcat(msg_env->data, receive_header);
    i =  RECEIVE_TRACE_BUF.head;
    do
    {
    	TraceLog* log = &RECEIVE_TRACE_BUF.trace_log[i];
    	char trace[100];

    	//sprintf(trace, "%i\t%i\t%i\t%s\t%i\n", count, log->dest_pid, log->sender_pid, msg_type(log->msg_type), log->time_stamp);
    	//strcat(msg_env->data, trace);
    	/*log_stack->dest_pid = log->dest_pid;
    	log_stack->msg_type = log->msg_type;
    	log_stack->sender_pid = log->sender_pid;
    	log_stack->time_stamp = log->time_stamp;*/
    	log_stack++;
    	i = (i+1)%TRACE_LOG_SIZE;
    }while(i!= receive_tail);

    return SUCCESS;
}

void k_print_buffer(MsgEnv *env)
{
	if (env && env->data)
	{
		// The location of the data should be: integer for send buffer count size, then count number of TraceLogs for send_buffer
		// integer for receive_buffer count size, then count number of TraceLogs for receive buffer
		//int send_size = *((int*)env->data);
		//TraceLog* log_stack =  sizeof(send_size);

	}
	else
	{
		ps("Print buffer failed due to NULL Arguments!");
	}
}
