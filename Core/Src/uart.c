#include "uart.h"
#include <string.h> // For strlen()
#include <stdio.h>	// For sprintf()
#include <inttypes.h>
// Global buffer for formatting strings
uint8_t msg[100];
uint8_t receive_buffer1 = 0;
uint8_t flag_buffer = 0;
uint16_t buffer_index = 0;
BufferState bufferState = STANDBY;

// Note: You must declare huart1 in uart.h or pass it as an argument
extern UART_HandleTypeDef huart1;

void uart_init_rs232()
{
	// Assuming receive_buffer1 is declared in uart.h or globally
	extern uint8_t receive_buffer1;
	HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
}

/**
 * @brief Sends a null-terminated string over UART.
 * @param str: The C-string (const char*) to send.
 */
void uart_Rs232SendString(const char *str)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), 10);
}

/**
 * @brief Sends raw bytes over UART.
 */
void uart_Rs232SendBytes(uint8_t *bytes, uint16_t size)
{
	HAL_UART_Transmit(&huart1, bytes, size, 10);
}

/**
 * @brief Formats and sends an unsigned 32-bit integer as a string.
 */
void uart_Rs232SendNum(uint32_t num)
{
	// %lu is the correct format specifier for uint32_t (unsigned long)
	int len = sprintf((char *)msg, "%lu", num);
	HAL_UART_Transmit(&huart1, msg, len, 10);
}

/**
 * @brief Formats and sends a number as "XX.YY".
 * @param num: An integer representing the value (e.g., 12345 -> "123.45")
 */
void uart_Rs232SendNumPercent(uint32_t num)
{
	// %02lu pads the fractional part with a leading zero (e.g., 5 -> "05")
	int len = sprintf((char *)msg, "%lu.%02lu", num / 100, num % 100);
	HAL_UART_Transmit(&huart1, msg, len, 10);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if (huart->Instance == USART1)
	{
		// rs232 isr
		// This is a simple echo-back
//		HAL_UART_Transmit(&huart1, &receive_buffer1, 1, 10);

		 fsm_uart();
		// Re-arm the receive interrupt
		HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
	}
}
void resetBuffer(void)
{
	for (uint16_t i = 0; i < 100; i++)
	{
		msg[i] = 0;
	}
}
void fsm_uart(void)
{
	switch (bufferState)
	{
	case STANDBY:
		// Đợi ký tự bắt đầu '#'
		if (receive_buffer1 == '#')
		{
			bufferState = STARTREAD;
			buffer_index = 0; // Reset chỉ số
		}
		break;

	case STARTREAD:
		// Đang ở giữa một tin nhắn
		if (receive_buffer1 == '#')
		{
			// Đã gặp ký tự kết thúc '#'
			msg[buffer_index] = '\0'; // 1. Thêm ký tự kết thúc chuỗi
			flag_buffer = 1;		  // 2. Bật cờ báo cho main loop
			bufferState = STANDBY;	  // 3. Quay lại chờ tin nhắn mới
		}
		else
		{
			// Vẫn là dữ liệu, thêm vào buffer
			msg[buffer_index] = receive_buffer1;
			buffer_index++;

			// Kiểm tra tràn bộ đệm (để 99 để chừa 1 chỗ cho '\0')
			if (buffer_index >= 99)
			{
				// Tràn, hủy tin nhắn này và reset
				bufferState = STANDBY;
				buffer_index = 0;
			}
		}
		break;

		// Trạng thái ENDREAD đã được loại bỏ vì không cần thiết

	default:
		// Trường hợp không xác định, quay về standby cho an toàn
		bufferState = STANDBY;
		break;
	}
	// if (receive_buffer1 == '#')
	// {
	// 	receive_buffer1 = ' ';
	// 	// avoid loop a case I think it can happen but not know clearly
	// }
}
