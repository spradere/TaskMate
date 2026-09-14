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
	if( hal_usartWriteChar((uint8_t)ch) == DRV_STATE_ERROR )
	{
		hal_driver_control_data_t control_data;
		hal_usartControl(DRV_CTRL_GETLASTERROR, &control_data);
		if( control_data.error == ERR_HAL_USART_TX_BUFFER_FULL )
		{
			hal_usartSendTXBuffer();
			hal_usartWriteChar((uint8_t)ch);
		}
	}

	if( ch == '\n' ) { hal_usartSendTXBuffer(); }
}

char sc_stringGetChar(const tm_string_t *str, uint8_t index)
{
	if( str->storage == TM_MEM_RAM ) { return str->text[index]; }
	if( str->storage == TM_MEM_ROM ) { return (char)pgm_read_byte(&(str->text[index])); }
	return 0;
}
