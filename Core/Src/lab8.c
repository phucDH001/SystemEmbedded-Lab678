#include "lab8.h"

void lab8_init() {
    sensor_init();
    uart_init_rs232();
    button_init();
    lcd_init();
	lcd_Clear(BLACK);
    timerInit();
    setTimer(0, 50);
    setTimer(1, 30000);
    sendTemperature();
}

void lab8_run() {
    if (checkTimerFlag(0)) {
        button_Scan();
        lightProcess();
        test_Esp();
    }
    if (checkTimerFlag(1)) {
        sendTemperature();
    }
}

uint8_t check_esp = 0;
uint8_t light_status = 0;

void lightProcess() {
    if (button_count[13] == 1) {
        light_status = 1 - light_status;
        if (light_status == 1)
            uart_Rs232SendBytes(&ctx_uart2, "A", 1);
        else
            uart_Rs232SendBytes(&ctx_uart2, "a", 1);
    }
    if (light_status == 1)
        HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 1);
    else
        HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
}

void test_Esp() {
    if (check_esp == 0) uart_Rs232SendBytes(&ctx_uart2, "o", 1);
    else lcd_ShowStr(10, 50, "ESP Connect", GREEN, BLACK, 24, 0);
}

void sendTemperature() {
    char buffer[32];
    float fl_temp = sensor_GetTemperature();
    int int_temp = (int)fl_temp;
    int dec_temp = (int)((fl_temp - int_temp) * 100);
    if (dec_temp < 0) dec_temp = -dec_temp;
    sprintf(buffer, "!TEMP:%d.%02d#", int_temp, dec_temp);
    uart_Rs232SendString(&ctx_uart2, buffer);
}
