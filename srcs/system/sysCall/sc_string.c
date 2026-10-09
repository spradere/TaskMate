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

#include <stddef.h>

#include "interfaces/drv_usart.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define SCSTRING_ORDER_LESS (-1)
#define SCSTRING_ORDER_GREATER 1

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
	if( (string == NULL) || (string->text == NULL) || (index >= TM_STRING_SIZE_MAX) )
	{
		return 0U;
	}

	switch( string->storage )
	{
		case TM_MEM_RAM:
			return string->text[index];
		case TM_MEM_ROM:
			return HAL_STRING_ROMGETBYTE(&(string->text[index]));
		default:
			return 0U;
	}
}

int sc_stringCompare(tm_string_t left, tm_string_t right, uint8_t size)
{
	for( uint8_t i = 0U; i < size; i++ )
	{
		uint8_t left_byte = sc_stringGetByte(&left, i);
		uint8_t right_byte = sc_stringGetByte(&right, i);

		if( left_byte < right_byte ) { return SCSTRING_ORDER_LESS; }
		if( left_byte > right_byte ) { return SCSTRING_ORDER_GREATER; }
		if( left_byte == 0U ) { return 0U; }
	}

	return 0U;
}

void sc_stringCopy(char *dest, tm_string_t src, uint8_t size)
{
	uint8_t i = 0U;

	if( (dest == NULL) || (size == 0U) ) { return; }
	if( src.text == NULL )
	{
		dest[0U] = 0U;
		return;
	}

	while( (i < (uint8_t)(size - 1U)) && (i < (TM_STRING_SIZE_MAX - 1U)) )
	{
		uint8_t src_byte = sc_stringGetByte(&src, i);
		if( src_byte == 0U ) { break; }
		dest[i] = (char)src_byte;
		i++;
	}
	dest[i] = 0U;
}
