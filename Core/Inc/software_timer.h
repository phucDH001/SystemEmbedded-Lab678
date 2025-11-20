#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "tim.h"

#define TIMER_IDX 10

extern int timer_counter[TIMER_IDX], timer_flag[TIMER_IDX];

void timerInit();
void setTimer(int, int);
int checkTimerFlag(int);
void timerRun();

#endif /* INC_SOFTWARE_TIMER_H_ */
