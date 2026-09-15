/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sc_string.c
 * @brief String syscall implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "sc_string.h"

#include "interfaces/drv_usart.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void sc_stdioPutChar(char ch)
{
	if( hal_usartWriteByte((uint8_t)ch) == DRV_STATE_ERROR )
	{
		hal_driver_control_data_t control_data;
		hal_usartControl(DRV_CTRL_GETLASTERROR, &control_data);
		if( control_data.error == ERR_HAL_USART_TX_BUFFER_FULL )
		{
			hal_usartSendTXBuffer();
			hal_usartWriteByte((uint8_t)ch);
		}
	}

	if( ch == '\n' ) { hal_usartSendTXBuffer(); }
}

tm_string_t sc_stringFromBuffer(const char *text)
{
	return (tm_string_t){.text = text, .storage = TM_MEM_RAM};
}

uint8_t sc_stringGetByte(const tm_string_t *string, uint8_t index)
{
	if( (string == 0) || (string->text == 0) ) { return 0; }
	if( string->storage == TM_MEM_RAM ) { return (uint8_t)string->text[index]; }
	if( string->storage == TM_MEM_ROM )
	{
		return (uint8_t)pgm_read_byte(&(string->text[index]));
	}
	return 0;
}

int sc_stringCompare(tm_string_t left, tm_string_t right, uint8_t size)
{
	for( uint8_t i = 0; i < size; i++ )
	{
		uint8_t left_byte = sc_stringGetByte(&left, i);
		uint8_t right_byte = sc_stringGetByte(&right, i);

		if( left_byte < right_byte ) { return -1; }
		if( left_byte > right_byte ) { return 1; }
		if( left_byte == 0 ) { return 0; }
	}

	return 0;
}

void sc_stringCopy(char *dest, tm_string_t src, uint8_t size)
{
	uint8_t i = 0;

	if( (dest == 0) || (size == 0) ) { return; }
	if( src.text == 0 )
	{
		dest[0] = 0;
		return;
	}

	while( (i < (uint8_t)(size - 1)) && (i < (TM_STRING_SIZE_MAX - 1)) )
	{
		uint8_t src_byte = sc_stringGetByte(&src, i);
		if( src_byte == 0 ) { break; }
		dest[i] = (char)src_byte;
		i++;
	}
	dest[i] = 0;
}
