#ifndef _CCI_H_
#define _CCI_H_

void cci_print(const char* print);
void setClock (int hour, int min, int sec);
void displayClock (int newStatus);

extern int clockDisplayRequest;
extern MsgEnvQ *envQ;
extern int hour, min, sec, clockTime;
extern MsgEnv* CCI_DISPLAY_ENV;

#endif
