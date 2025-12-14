/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

typedef enum
{
	ERROR_LOW,
	ERROR_MID,
	ERROR_HIGH
} error_critical_t;

typedef enum
{
	ERR_NO_ERROR,
	ERR_HAL_USART_TX_BUFFER_EMPTY,
	ERR_HAL_USART_RX_BUFFER_EMPTY,
	ERR_HAL_USART_TX_BUFFER_FULL,
	ERR_HAL_USART_RX_BUFFER_FULL,
	ERR_MSG_OUT_OF_FREE_CHANNEL,
	ERROR_COUNT
} error_codes_t;typedef struct
{
	char *name;
	uint8_t critical;
} error_item_t;

const error_item_t error_catalog[] = 
{
	{"No error", 1},
	{"TX buffer is empty", 1},
	{"RX buffer is empty", 1},
	{"TX buffer is full", 1},
	{"RX buffer is full", 1},
	{"No more free channels", 1}
};
