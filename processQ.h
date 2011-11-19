#ifndef _PROCESSQ_HEADER_
#define _PROCESSQ_HEADER_

#include "global_structs.h"

proc_queue * proc_q_create();
void proc_q_destroy(proc_queue * queue);

int proc_q_is_empty(proc_queue * queue);
pcb * proc_q_dequeue(proc_queue * queue);
int proc_q_enqueue(proc_queue * queue, pcb * pcb);
pcb * proc_q_remove(proc_queue * queue, pcb *pcb);

#endif
