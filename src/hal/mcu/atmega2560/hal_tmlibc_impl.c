/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_tmlibc_impl.c
 * @brief atmega2560 hal_tmlibc_impl implementation.
 *
 */

#include "hal/mcu/atmega2560/hal_tmlibc_impl.h"

#include "hal/mcu/atmega2560/hal_usart_impl.h"

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
