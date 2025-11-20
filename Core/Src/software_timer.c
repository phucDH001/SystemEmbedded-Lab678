#include "software_timer.h"

int timer_counter[TIMER_IDX] = {0};
int timer_buffer[TIMER_IDX] = {0};
int timer_flag[TIMER_IDX] = {0};
int TIMER_CYCLE = 1;

void timerInit() {
	HAL_TIM_Base_Start_IT(&htim2);
}

void setTimer(int idx, int duration) {
	timer_counter[idx] = duration / TIMER_CYCLE;
	timer_buffer[idx] = duration / TIMER_CYCLE;
	timer_flag[idx] = 0;
}

int checkTimerFlag(int idx) {
	if (timer_flag[idx] == 1) {
		timer_flag[idx] = 0;
		return 1;
	}
	return 0;
}

void timerRun() {
	for (int i = 0; i < TIMER_IDX; i++) {
		if (timer_counter[i] > 0) {
			timer_counter[i]--;
			if (timer_counter[i] == 0) {
				timer_flag[i] = 1;
				timer_counter[i] = timer_buffer[i];
			}
		}
	}
}
