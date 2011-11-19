#include "rtx.h"
#include "timerQ.h"
#include "iProcs.h"

void timeout_q_insert (MsgEnv* new_msg_env)
{
    // assume new_msg_env != NULL
    //assert(new_msg_env != NULL);

	//calculate the time with respect to the system time
    int timeout = clock_get_time() + new_msg_env->time_delay;
    new_msg_env->time_delay = timeout;

    // Check for empty queue
    if (TIMEOUT_Q == NULL)
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
    }

    // Find the insertion point
    MsgEnv* prev_node = node;
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
    }

    // Insert into the queue
    prev_node->next = new_msg_env;
    new_msg_env->next = node;
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
