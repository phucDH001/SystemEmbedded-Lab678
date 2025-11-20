#ifndef INC_LAB6_H_
#define INC_LAB6_H_

#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "sensor.h"
#include "buzzer.h"
#include "uart.h"

#define POWER_MIN   0      // Giá trị power nhỏ nhất
#define POWER_MAX   70000    // Giá trị power lớn nhất

// ---- TỌA ĐỘ VÙNG VẼ (cho màn hình 240x320) ----
#define CHART_X_START 30  // Chừa 30px bên trái cho nhãn trục Y
#define CHART_Y_START 120 // Chừa 40px bên trên cho tiêu đề
#define CHART_X_END   230 // Cạnh phải (240 - 10px lề)
#define CHART_Y_END   300 // Cạnh dưới (320 - 20px lề)

#define CHART_WIDTH   (CHART_X_END - CHART_X_START) // 200 pixels
#define CHART_HEIGHT  (CHART_Y_END - CHART_Y_START) // 260 pixels

void lab6_init();
void lab6_run();
void task_adc();
void task_buzzer();
void task_update_led7();
void task_get_power();

void chart_init();
void task_chart_update();

#endif /* INC_LAB6_H_ */
