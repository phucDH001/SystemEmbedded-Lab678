#ifndef INC_UART_H_
#define INC_UART_H_

#include "usart.h"
#include <stdio.h>
#include "utils.h"

typedef enum
{
    STANDBY,
    STARTREAD,
    ENDREAD
} BufferState;

extern uint8_t receive_buffer1;
extern uint8_t flag_buffer;
extern uint16_t buffer_index;
extern uint8_t msg[];
void uart_init_rs232();

void uart_Rs232SendString(const char *str);

void uart_Rs232SendBytes(uint8_t *bytes, uint16_t size);

void uart_Rs232SendNum(uint32_t num);

void uart_Rs232SendNumPercent(uint32_t num);

void fsm_uart(void);

#endif /* INC_UART_H_ */
