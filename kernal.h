#ifndef _KERNAL_HEADER_
#define _KERNAL_HEADER_

#include "rtx.h"

void atomic(bool_t state);

pcb* pid_to_pcb(int dest_process_id);

int k_send_message(int dest_process_id, MsgEnv *msg_envelope);
MsgEnv * k_receive_message();
int k_get_console_chars(MsgEnv *message_envelope);
int k_send_console_chars(MsgEnv *message_envelope);

int k_release_message_env(MsgEnv* env);
MsgEnv* k_request_msg_env();

int k_pseudo_process_switch(int pid);
void k_return_from_switch();

int k_request_delay(int delay, int wakeup_code, MsgEnv *msg_env);

void k_process_switch(ProcessState next_state);
void k_context_switch(jmp_buf prev, jmp_buf next);

int k_release_processor();
int k_request_process_status(MsgEnv *env);
int k_terminate();
int k_change_priority(int new_priority, int target_pid);

int k_log_event(TraceBuffer* trace_buf, MsgEnv *env);
int k_get_trace_buffer(MsgEnv* env);

void k_print_buffer(MsgEnv* env);

#endif
