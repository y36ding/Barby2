#include "rtx.h"
#include "timerQ.h"
#include "iProcs.h"

void timeout_q_insert (MsgEnv* insert_env)
{
    // assume new_msg_env != NULL
    assert(insert_env != NULL);

	//calculate the delay finish time with respect to the system time
    /*int timeout = clock_get_time() + new_msg_env->time_delay;
    new_msg_env->time_delay = timeout;*/

	//get_RTX_time() perhaps?
	insert_env->time_delay = clock_get_time()+((insert_env->time_delay)*10);


    // Check for empty queue
    /*if (TIMEOUT_Q == NULL)
    {
    	TIMEOUT_Q = new_msg_env;
        return;
    }

    // Insert at head of queue
    MsgEnv* node = TIMEOUT_Q;
    int timeout_so_far = *((int*)node->data);
    if (timeout <= *((int *) node->data))
    {
        new_msg_env->next = node;
        TIMEOUT_Q = new_msg_env;
        return;
    }*/

	// "||" or "|" for or operator? it shouldnt check 2nd cond if 1st is true
	MsgEnv * queue_head = TIMEOUT_Q;
	if(queue_head == NULL||(insert_env->time_delay < queue_head->time_delay))
	{
		insert_env->next = queue_head;
		TIMEOUT_Q = insert_env;
		return;
	}

    // Find the insertion point
    /*MsgEnv* prev_node = node;
    node = node->next;
    if (node)
    {
        timeout_so_far = *((int*)node->data);
        while(timeout_so_far < timeout && node != NULL)
        {
            prev_node = node;
            node = node->next;
            timeout_so_far = *((int*)node->data);
        }
    }*/

	MsgEnv* prev_env = queue_head;
	MsgEnv* env = queue_head->next;

	if(env!=NULL)
	{
		while((insert_env->time_delay > env->time_delay) && env!=NULL)
		{
			prev_env=env;
			env=env->next;
		}
	}

    // Insert into the queue
    prev_env->next = insert_env;
    insert_env->next = env;
}

int timeout_q_is_empty()
{
	//check if the timeout queue is null or not.
    return TIMEOUT_Q == NULL;
}

MsgEnv * get_timeout_q()
{
	return TIMEOUT_Q;
}

MsgEnv * check_timeout_q()
{
	if(!timeout_q_is_empty()) {
	while (TIMEOUT_Q && (TIMEOUT_Q->time_delay) <= clock_get_time()) {
		// Dequeue the head
		MsgEnv* msg_env = TIMEOUT_Q;
		TIMEOUT_Q = TIMEOUT_Q->next;
		return msg_env;
		}
	}
	return NULL;
}
