#ifndef _CCI_H_
#define _CCI_H_

void cci_print(const char* print);
int setClock (char* rawTimeString);
void displayClock (int newStatus);

extern int clockDisplayRequest;
extern MsgEnvQ *envQ;
extern int hour, min, sec, clockTime;

#endif
