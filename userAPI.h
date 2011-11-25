#ifndef _USERAPI_HEADER_
#define _USERAPI_HEADER_

#include "global_structs.h"

// Base primitives
int send_message(int dest_pid, MsgEnv *msg_env);
MsgEnv *receive_message();
MsgEnv* request_msg_env();
int release_message_env(MsgEnv* env);
int release_processor();
int request_process_status(MsgEnv *env);
int terminate();
int change_priority(int new_priority, int target_pid);
int request_delay(int delay, int wakeup_code, MsgEnv *msg_env);
int send_console_chars(MsgEnv *msg_env);
int get_console_chars(MsgEnv *msg_env);
int get_trace_buffers(MsgEnv *env);

// Extra user process for release
int release_processor();

#endif
