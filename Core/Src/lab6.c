#include "lab6.h"

void lab6_init() {
    timerInit();
    button_init();
    led7_init();
    lcd_init();
    ds3231_init();
    sensor_init();
    buzzer_init();
    setTimer(0, 50);
    setTimer(1, 1000);
    setTimer(2, 15000);
	ds3231_Write(ADDRESS_HOUR, 12);
	ds3231_Write(ADDRESS_MIN, 34);
	ds3231_Write(ADDRESS_SEC, 56);
	ds3231_ReadTime();
	task_update_led7();
	lcd_Clear(BLACK);
	chart_init();
	task_get_power();
	task_adc();
	task_chart_update();
}

void lab6_run() {
	if (checkTimerFlag(0)) {
        button_Scan();
        task_adc();
	}
	if (checkTimerFlag(1)) {
        task_buzzer();
        task_update_led7();
        task_chart_update();
	}
	if (checkTimerFlag(2)) {
		task_get_power();
	}
}

uint8_t count_adc = 0;
float power, humi;
void task_adc() {
    count_adc = (count_adc + 1) % 20;
    if (count_adc == 0) {
        sensor_Read();
        lcd_ShowStr(30, 10, "Power:", WHITE, BLACK, 16, 0);
        lcd_ShowFloatNum(130, 10, power, 7, WHITE, BLACK, 16);
        lcd_ShowStr(195, 10, "mW", WHITE, BLACK, 16, 0);
        
        char* light = (sensor_GetLight() > 2048) ? "Strong" : "Weak  ";
        lcd_ShowStr(30, 30, "Light:", WHITE, BLACK, 16, 0);
        lcd_ShowStr(130, 30, light, WHITE, BLACK, 16, 0);

        lcd_ShowStr(30, 50, "Temperature:", WHITE, BLACK, 16, 0);
        lcd_ShowFloatNum(130, 50, sensor_GetTemperature(), 4, WHITE, BLACK, 16);
        lcd_ShowStr(170, 50, "*C", WHITE, BLACK, 16, 0);

        humi = sensor_GetPotentiometer() / 4095.0 * 100.0;
        if (humi > 70) {
			lcd_ShowStr(30, 70, "Humidity:", RED, BLACK, 16, 0);
			lcd_ShowFloatNum(130, 70, humi, 4, RED, BLACK, 16);
			lcd_ShowStr(170, 70, "%", RED, BLACK, 16, 0);
        }
        else {
			lcd_ShowStr(30, 70, "Humidity:", WHITE, BLACK, 16, 0);
			lcd_ShowFloatNum(130, 70, humi, 4, WHITE, BLACK, 16);
			lcd_ShowStr(170, 70, "%", WHITE, BLACK, 16, 0);
        }
    }
}

void task_get_power() { power = sensor_GetCurrent() * sensor_GetVoltage(); }

int alert_state = 1;
void task_buzzer() {
    if (humi > 70.0) {
        if (alert_state) {
        	buzzer_SetVolume(50);
        	uart_Rs232SendString(&ctx_uart1, "Humidity exceeds threshold!!!\n");
        }
        else buzzer_SetVolume(0);
        alert_state = 1 - alert_state;
    }
    else buzzer_SetVolume(0);
}

int colon_state = 1;
void toggle_colon() {
	led7_SetColon(colon_state);
	colon_state = 1 - colon_state;
}

void task_update_led7() {
	ds3231_ReadTime();
	led7_SetDigit(ds3231_hours / 10, 0, 0);
	led7_SetDigit(ds3231_hours % 10, 1, 0);
	led7_SetDigit(ds3231_min / 10, 2, 0);
	led7_SetDigit(ds3231_min % 10, 3, 0);
	toggle_colon();
}

static uint16_t g_current_x = CHART_X_START;
static uint16_t g_last_y = CHART_Y_END;
static uint16_t scale_power_to_y() {
	// Converts a raw power value into a Y-coordinate for the display
    if (power < POWER_MIN) power = POWER_MIN;
    if (power > POWER_MAX) power = POWER_MAX;
    long y_offset = ((long)(power - POWER_MIN) * CHART_HEIGHT) / (POWER_MAX - POWER_MIN);
    return CHART_Y_END - (uint16_t)y_offset;
}

void chart_init() {
	// Initializes the visual layout of the chart
    lcd_ShowStr(90, CHART_Y_START - 20, "Power Consumption", YELLOW, BLACK, 16, 0);
    lcd_DrawRectangle(CHART_X_START - 5, CHART_Y_START, CHART_X_END - 5, CHART_Y_END, WHITE);
    lcd_ShowStr(5, CHART_Y_START - 20, "70000", WHITE, BLACK, 16, 0);
    lcd_ShowStr(10, CHART_Y_END - 16, "0", WHITE, BLACK, 16, 0);
    g_current_x = CHART_X_START;
    g_last_y = CHART_Y_END;
    lcd_Fill(CHART_X_START - 5 + 1, CHART_Y_START + 1, CHART_X_END - 5 - 1, CHART_Y_END, BLACK);
}

void task_chart_update() {
	// Updates the real-time graph with a new data point
    uint16_t new_y = scale_power_to_y();
    // If the graph reaches the right edge of the screen, it clears the chart area and resets the drawing position
    if (g_current_x >= CHART_X_END - 10) {
        g_current_x = CHART_X_START;
        lcd_Fill(CHART_X_START - 5 + 1, CHART_Y_START + 1, CHART_X_END - 5 - 1, CHART_Y_END, BLACK);
        lcd_DrawRectangle(CHART_X_START - 5, CHART_Y_START, CHART_X_END - 5, CHART_Y_END, WHITE);
        lcd_DrawPoint(g_current_x, new_y, GREEN);
    }
    else {
        lcd_DrawLine(g_current_x, g_last_y, g_current_x + 1, new_y, GREEN);
        g_current_x++;
    }
    g_last_y = new_y;
}
