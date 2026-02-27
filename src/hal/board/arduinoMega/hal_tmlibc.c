/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file hal_tmlibc.c
 * @brief board stdio
 *
 */

#include "hal/board/arduinoMega/hal_tmlibc.h"

#include "hal/auto_hal_user.h"

void hal_stdio_putChar(char ch)
{
	if( hal_usartWriteChar((uint8_t)ch) == ERR_HAL_USART_TX_BUFFER_FULL )
	{
		hal_usartSendTXBuffer();
		hal_usartWriteChar((uint8_t)ch);
	}

	if( ch == '\n' ) { hal_usartSendTXBuffer(); }
}

char hal_string_getChar(const tm_string_t *str, uint8_t index)
{
	if( str->storage == TM_MEM_RAM ) { return str->text[index]; }
	if( str->storage == TM_MEM_ROM ) { return (char)pgm_read_byte(&(str->text[index])); }
	return 0;
}
