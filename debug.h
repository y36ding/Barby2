#ifndef _DEBUG_H
#define _DEBUG_H_

#include "global_structs.h"

void pm(MsgEnv* env);
void pf(double val);
void pi(int val);
void ps(char* val);
void pp(pcb* val);
void pstacks();

#endif
