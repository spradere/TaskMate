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

uint8_t sc_stringGetByte(const tm_string_t *string, size_t index)
{
#if TM_LIBC_TASKMATE
	if( (string == NULL) || (string->text == NULL) ) { return 0U; }

	switch( string->storage )
	{
		case TM_MEM_RAM:
			return string->text[index];
		case TM_MEM_ROM:
			return HAL_STRING_ROMGETBYTE(&(string->text[index]));
		default:
			return 0U;
	}
#else
	if( string == NULL ) { return 0U; }
	return (uint8_t)string[index];
#endif
}
