#include "uart.h"
#include <string.h> // For strlen()
#include <stdio.h>	// For sprintf()
#include <inttypes.h>

// Initialize a message buffer
UartContext_t ctx_uart1 = { .huart = &huart1, .rx_byte = 0, .state = STANDBY, .index = 0, .flag = 0 };
UartContext_t ctx_uart2 = { .huart = &huart2, .rx_byte = 0, .state = STANDBY, .index = 0, .flag = 0 };

void uart_init_rs232() {
	HAL_UART_Receive_IT(ctx_uart1.huart, &ctx_uart1.rx_byte, 1);
    HAL_UART_Receive_IT(ctx_uart2.huart, &ctx_uart2.rx_byte, 1);
}

/**
 * @brief Sends a null-terminated string over UART.
 * @param str: The C-string (const char*) to send.
 */
void uart_Rs232SendString(UartContext_t *ctx, const char *str) {
	HAL_UART_Transmit(ctx->huart, (uint8_t *)str, strlen(str), 10);
}

/**
 * @brief Sends raw bytes over UART.
 */
void uart_Rs232SendBytes(UartContext_t *ctx, uint8_t *bytes, uint16_t size) {
	HAL_UART_Transmit(ctx->huart, bytes, size, 10);
}

/**
 * @brief Formats and sends an unsigned 32-bit integer as a string.
 */
void uart_Rs232SendNum(UartContext_t *ctx, uint32_t num) {
    char temp_str[32]; // Buffer cục bộ, an toàn, không lo xung đột với RX
    int len = sprintf(temp_str, "%lu", num);
    HAL_UART_Transmit(ctx->huart, (uint8_t *)temp_str, len, 10);
}

/**
 * @brief Formats and sends a number as "XX.YY".
 * @param num: An integer representing the value (e.g., 12345 -> "123.45")
 */
void uart_Rs232SendNumPercent(UartContext_t *ctx, uint32_t num) {
    char temp_str[32];
    int len = sprintf(temp_str, "%lu.%02lu", num / 100, num % 100);
    HAL_UART_Transmit(ctx->huart, (uint8_t *)temp_str, len, 10);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	UartContext_t *ctx = NULL;
    
    if (huart->Instance == USART1) ctx = &ctx_uart1;
    else if (huart->Instance == USART2) ctx = &ctx_uart2;
    
    if (ctx != NULL) {
        fsm_uart(ctx);
        HAL_UART_Receive_IT(ctx->huart, &ctx->rx_byte, 1);
    }
}

void reset_buffer(UartContext_t *ctx) {
    memset(ctx->msg, 0, sizeof(ctx->msg)); 
    ctx->index = 0;
}

void fsm_uart(UartContext_t *ctx) {
	if (ctx->huart->Instance == USART1) {
        // Process data received from UART1 if needed
	}
	else if (ctx->huart->Instance == USART2) {
		if(ctx->rx_byte == 'O') check_esp = 1;
		else if(ctx->rx_byte == 'a') light_status = 0;
		else if(ctx->rx_byte == 'A') light_status = 1;
	}
    switch (ctx->state) {
        case STANDBY:
            if (ctx->rx_byte == '!') {
                ctx->state = STARTREAD;
                ctx->index = 0;
            }
            break;
        case STARTREAD:
            if (ctx->rx_byte == '#') {
                ctx->msg[ctx->index] = '\0';
                ctx->flag = 1;
                ctx->state = STANDBY;
            }
            else if (ctx->rx_byte == '!') ctx->index = 0; 
            else {
                ctx->msg[ctx->index] = ctx->rx_byte;
                ctx->index++;
                if (ctx->index >= 99) {
                    ctx->state = STANDBY;
                    ctx->index = 0;
                }
            }
            break;
        default:
            ctx->state = STANDBY;
            break;
    }
}
