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

err_codes_t sc_consoleWriteByte(uint8_t data)
{
	if( hal_usartWriteByte(data) == DRV_STATE_RUNNING ) { return ERR_NO_ERROR; }

	hal_driver_control_data_t control_data;
	hal_usartControl(DRV_CTRL_GETLASTERROR, &control_data);
	if( control_data.error != ERR_HAL_USART_TX_BUFFER_FULL ) { return control_data.error; }

	err_codes_t error = sc_consoleFlush();
	if( error != ERR_NO_ERROR ) { return error; }
	if( hal_usartWriteByte(data) == DRV_STATE_RUNNING ) { return ERR_NO_ERROR; }

	hal_usartControl(DRV_CTRL_GETLASTERROR, &control_data);
	return control_data.error;
}

err_codes_t sc_consoleFlush(void)
{
	if( hal_usartSendTXBuffer() == DRV_STATE_RUNNING ) { return ERR_NO_ERROR; }

	hal_driver_control_data_t control_data;
	hal_usartControl(DRV_CTRL_GETLASTERROR, &control_data);
	return control_data.error;
}

uint8_t sc_stringGetByte(const tm_string_t *string, uint8_t index)
{
	if( (string == 0) || (string->text == 0) || (index >= TM_STRING_SIZE_MAX) ) { return 0; }

	switch( string->storage )
	{
		case TM_MEM_RAM:
			return (uint8_t)string->text[index];
		case TM_MEM_ROM:
			//return (uint8_t)pgm_read_byte(&(string->text[index]));
			return (uint8_t)HAL_STRING_ROMGETBYTE(&(string->text[index]));
		default:
			return 0;
	}
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
