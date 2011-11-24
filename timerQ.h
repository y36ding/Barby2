#ifndef _TIMERQ_HEADER_
#define _TIMERQ_HEADER_

#include "global_structs.h"

int timeout_q_is_empty();
void timeout_q_insert (MsgEnv* insert_env);
MsgEnv * check_timeout_q();
MsgEnv * get_timeout_q();

#endif
