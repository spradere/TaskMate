/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_freebsd_usart.c
 * @brief Exercise the FreeBSD USART control and C ABI from a C caller.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "hal/board/pc/pc_console.h"
#include "interfaces/drv_usart.h"
#include "interfaces/hal_atomic.h"

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static bool console_init_ok = true;
static bool console_have_byte;
static uint8_t console_byte;
static uint16_t console_written;
static uint16_t console_flushes;
static uint16_t atomic_entries;
static uint16_t atomic_exits;

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

bool pc_consoleInit(void) { return console_init_ok; }

bool pc_consoleReadByte(uint8_t *data)
{
	if( !console_have_byte ) { return false; }
	*data = console_byte;
	console_have_byte = false;
	return true;
}

void pc_consoleWriteByte(uint8_t data)
{
	assert(data == (uint8_t)console_written);
	console_written++;
}

void pc_consoleFlush(void) { console_flushes++; }

hal_atomic_state_t hal_atomicStart(void)
{
	atomic_entries++;
	return atomic_entries;
}

void hal_atomicEnd(hal_atomic_state_t state)
{
	assert(state == atomic_entries);
	atomic_exits++;
}

static err_codes_t lastError(void)
{
	hal_driver_control_data_t data;
	(void)hal_usartControl(DRV_CTRL_GETLASTERROR, &data);
	return data.error;
}

int main(void)
{
	hal_driver_control_data_t data;
	uint8_t byte = 0U;

	assert(hal_usartControl(DRV_CTRL_GETSTATUS, 0U) == DRV_STATE_OFF);
	assert(lastError() == ERR_NO_ERROR);
	assert(hal_usartControl(DRV_CTRL_START, 0U) == DRV_STATE_ERROR);
	assert(lastError() == ERR_HAL_DRIVER_NOT_INITIALIZED);
	assert(hal_usartControl(DRV_CTRL_RLSET, 0U) == DRV_STATE_ERROR);
	assert(lastError() == ERR_NULL_POINTER);

	console_init_ok = false;
	assert(hal_usartControl(DRV_CTRL_INIT, 0U) == DRV_STATE_ERROR);
	assert(lastError() == ERR_HAL_DRIVER_DEPENDENCY);
	console_init_ok = true;
	assert(hal_usartControl(DRV_CTRL_INIT, 0U) == DRV_STATE_INITIALIZED);
	assert(lastError() == ERR_NO_ERROR);
	assert(hal_usartRead(&byte) == DRV_STATE_ERROR);
	assert(lastError() == ERR_HAL_DRIVER_NOT_RUNNING);
	assert(hal_usartControl(DRV_CTRL_START, 0U) == DRV_STATE_RUNNING);
	assert(hal_usartControl(DRV_CTRL_RLGET, 0U) == DRV_STATE_ERROR);
	assert(lastError() == ERR_NULL_POINTER);
	assert(hal_usartControl(DRV_CTRL_GETLASTERROR, 0U) == DRV_STATE_ERROR);
	assert(lastError() == ERR_NULL_POINTER);
	assert(hal_usartControl(DRV_CTRL_SETBIT, 0U) == DRV_STATE_ERROR);
	assert(hal_usartControl(DRV_CTRL_CLEARBIT, 0U) == DRV_STATE_ERROR);
	assert(hal_usartControl(DRV_CTRL_GETBIT, 0U) == DRV_STATE_ERROR);
	assert(lastError() == ERR_NULL_POINTER);

	assert(hal_usartRead(0U) == DRV_STATE_ERROR);
	assert(lastError() == ERR_NULL_POINTER);
	assert(hal_usartRead(&byte) == DRV_STATE_ERROR);
	assert(lastError() == ERR_HAL_USART_RX_BUFFER_EMPTY);
	console_have_byte = true;
	console_byte = 42U;
	assert(hal_usartRead(&byte) == DRV_STATE_RUNNING);
	assert(byte == 42U);
	assert(atomic_entries == 2U);
	assert(atomic_exits == 2U);

	for( uint16_t i = 0U; i < 256U; i++ )
	{
		assert(hal_usartWriteByte((uint8_t)i) == DRV_STATE_RUNNING);
	}
	assert(hal_usartWriteByte(0U) == DRV_STATE_ERROR);
	assert(lastError() == ERR_HAL_USART_TX_BUFFER_FULL);
	assert(hal_usartSendTXBuffer() == DRV_STATE_RUNNING);
	assert(console_written == 256U);
	assert(console_flushes == 1U);
	assert(atomic_entries == 3U);
	assert(atomic_exits == 3U);

	data.run_level = RL_RUN_SERVICE;
	assert(hal_usartControl(DRV_CTRL_RLSET, &data) == DRV_STATE_RUNNING);
	assert(hal_usartControl(DRV_CTRL_RLGET, &data) == DRV_STATE_RUNNING);
	assert(data.run_level == RL_RUN_SERVICE);
	data.run_level = RL_LEVEL_COUNT;
	assert(hal_usartControl(DRV_CTRL_RLSET, &data) == DRV_STATE_ERROR);
	assert(lastError() == ERR_HAL_DRIVER_INVALID_VALUE);
	data.status_bit = (hal_driver_status_bit_t)DRV_BIT_ERROR;
	assert(hal_usartControl(DRV_CTRL_GETBIT, &data) == DRV_STATE_RUNNING);
	assert(!data.bit_value);
	data.status_bit = (hal_driver_status_bit_t)2U;
	assert(hal_usartControl(DRV_CTRL_SETBIT, &data) == DRV_STATE_ERROR);
	assert(lastError() == ERR_HAL_DRIVER_INVALID_VALUE);
	data.status_bit = (hal_driver_status_bit_t)7U;
	assert(hal_usartControl(DRV_CTRL_CLEARBIT, &data) == DRV_STATE_ERROR);
	assert(lastError() == ERR_HAL_DRIVER_INVALID_VALUE);
	data.status_bit = DRV_BIT_ERROR;
	assert(hal_usartControl(DRV_CTRL_SETBIT, &data) == DRV_STATE_ERROR);
	assert(lastError() == ERR_HAL_DRIVER_INVALID_STATE);
	data.status_bit = DRV_BIT_DEAD;
	assert(hal_usartControl(DRV_CTRL_SETBIT, &data) == DRV_STATE_DEAD);
	assert(lastError() == ERR_HAL_DRIVER_DEAD);
	assert(hal_usartControl(DRV_CTRL_CLEARBIT, &data) == DRV_STATE_ERROR);
	assert(lastError() == ERR_HAL_DRIVER_INVALID_STATE);
	data.status_bit = DRV_BIT_ERROR;
	assert(hal_usartControl(DRV_CTRL_CLEARBIT, &data) == DRV_STATE_RUNNING);
	assert(hal_usartControl((hal_driver_control_t)255U, 0U) == DRV_STATE_ERROR);
	assert(lastError() == ERR_HAL_DRIVER_INVALID_CONTROL);
	assert(hal_usartControl(DRV_CTRL_STOP, 0U) == DRV_STATE_INITIALIZED);
	assert(hal_usartSendTXBuffer() == DRV_STATE_ERROR);
	assert(lastError() == ERR_HAL_DRIVER_NOT_RUNNING);
	return 0;
}
