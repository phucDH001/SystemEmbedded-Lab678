#ifndef INC_UART_H_
#define INC_UART_H_

#include "usart.h"
#include <stdio.h>
#include "utils.h"
#include "lab8.h"

typedef enum
{
    STANDBY,
    STARTREAD,
    ENDREAD
} BufferState;

typedef struct {
    UART_HandleTypeDef *huart;
    
    uint8_t msg[100];
    uint8_t rx_byte;
    
    uint16_t index;
    uint8_t flag;
    BufferState state;
} UartContext_t;

extern UartContext_t ctx_uart1, ctx_uart2;

void uart_init_rs232();

void uart_Rs232SendString(UartContext_t *ctx, const char *str);

void uart_Rs232SendBytes(UartContext_t *ctx, uint8_t *bytes, uint16_t size);

void uart_Rs232SendNum(UartContext_t *ctx, uint32_t num);

void uart_Rs232SendNumPercent(UartContext_t *ctx, uint32_t num);

void fsm_uart(UartContext_t *ctx);

#endif /* INC_UART_H_ */
