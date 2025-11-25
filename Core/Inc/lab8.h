#ifndef INC_LAB8_H_
#define INC_LAB8_H_

#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "sensor.h"
#include "buzzer.h"
#include "uart.h"

extern uint8_t check_esp, light_status;

void lightProcess();
void test_Esp();
void sendTemperature();

void lab8_init();
void lab8_run();

#endif /* INC_LAB8_H_ */
