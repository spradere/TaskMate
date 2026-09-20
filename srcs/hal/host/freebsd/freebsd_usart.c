/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file freebsd_usart.c
 * @brief FreeBSD virtual USART implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "interfaces/drv_usart.h"

#include <stdint.h>

#include "hal/board/pc/pc_console.h"
#include "interfaces/hal_atomic.h"
#include "interfaces/tm_macros.h"
#include "interfaces/tm_runLevel.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define FREEBSD_USART_TX_BUFFER_SIZE 256u

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static uint8_t tx_buffer[FREEBSD_USART_TX_BUFFER_SIZE];
static uint16_t tx_length;
static hal_driver_status_t usart_status;
static err_codes_t usart_last_error = ERR_NO_ERROR;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static hal_driver_state_t usartSetError(err_codes_t error);
static hal_driver_state_t usartGetStatus(void);
static hal_driver_state_t usartRequireRunning(void);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static hal_driver_state_t usartSetError(err_codes_t error)
{
	usart_last_error = error;
	return DRV_STATE_ERROR;
}

static hal_driver_state_t usartGetStatus(void)
{
	if( TM_GETBIT(usart_status, DRV_BIT_DEAD) != 0 )
	{
		usart_last_error = ERR_HAL_DRIVER_DEAD;
		return DRV_STATE_DEAD;
	}
	if( TM_GETBIT(usart_status, DRV_BIT_ERROR) != 0 )
	{
		return usartSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( TM_GETBIT(usart_status, DRV_BIT_INIT) == 0 ) { return DRV_STATE_OFF; }
	if( TM_GETBIT(usart_status, DRV_BIT_START) == 0 ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t usartRequireRunning(void)
{
	hal_driver_state_t state = usartGetStatus();
	if( (state == DRV_STATE_OFF) || (state == DRV_STATE_INITIALIZED) )
	{
		return usartSetError(ERR_HAL_DRIVER_NOT_RUNNING);
	}
	return state;
}

static hal_driver_state_t usartInit(void)
{
	if( !pc_consoleInit() ) { return usartSetError(ERR_HAL_DRIVER_DEPENDENCY); }
	tx_length = 0;
	TM_SETBIT(usart_status, DRV_BIT_INIT);
	usart_last_error = ERR_NO_ERROR;
	return DRV_STATE_INITIALIZED;
}

static hal_driver_state_t usartStart(void)
{
	if( TM_GETBIT(usart_status, DRV_BIT_INIT) == 0 )
	{
		return usartSetError(ERR_HAL_DRIVER_NOT_INITIALIZED);
	}
	TM_SETBIT(usart_status, DRV_BIT_START);
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t usartStop(void)
{
	TM_CLEARBIT(usart_status, DRV_BIT_START);
	return usartGetStatus();
}

hal_driver_state_t hal_usartRead(uint8_t *data)
{
	hal_driver_state_t state = usartRequireRunning();
	if( state != DRV_STATE_RUNNING ) { return state; }
	if( data == 0 ) { return usartSetError(ERR_NULL_POINTER); }

	hal_atomic_state_t atomic_state = hal_atomicStart();
	bool have_data = pc_consoleReadByte(data);
	hal_atomicEnd(atomic_state);
	if( !have_data ) { return usartSetError(ERR_HAL_USART_RX_BUFFER_EMPTY); }
	return DRV_STATE_RUNNING;
}

hal_driver_state_t hal_usartWriteByte(uint8_t data)
{
	hal_driver_state_t state = usartRequireRunning();
	if( state != DRV_STATE_RUNNING ) { return state; }
	if( tx_length >= FREEBSD_USART_TX_BUFFER_SIZE )
	{
		return usartSetError(ERR_HAL_USART_TX_BUFFER_FULL);
	}
	tx_buffer[tx_length++] = data;
	return DRV_STATE_RUNNING;
}

hal_driver_state_t hal_usartSendTXBuffer(void)
{
	hal_driver_state_t state = usartRequireRunning();
	if( state != DRV_STATE_RUNNING ) { return state; }

	hal_atomic_state_t atomic_state = hal_atomicStart();
	for( uint16_t i = 0; i < tx_length; i++ ) { pc_consoleWriteByte(tx_buffer[i]); }
	pc_consoleFlush();
	tx_length = 0;
	hal_atomicEnd(atomic_state);
	return DRV_STATE_RUNNING;
}

hal_driver_state_t hal_usartControl(hal_driver_control_t command, hal_driver_control_data_t *data)
{
	switch( command )
	{
		case DRV_CTRL_INIT:
			return usartInit();
		case DRV_CTRL_START:
			return usartStart();
		case DRV_CTRL_STOP:
			return usartStop();
		case DRV_CTRL_RLSET:
			if( data == 0 ) { return usartSetError(ERR_NULL_POINTER); }
			if( data->run_level >= RL_LEVEL_COUNT )
			{
				return usartSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			usart_status &= (hal_driver_status_t)~RL_LEVEL_MASK;
			usart_status |= data->run_level;
			return usartGetStatus();
		case DRV_CTRL_RLGET:
			if( data == 0 ) { return usartSetError(ERR_NULL_POINTER); }
			data->run_level = usart_status & RL_LEVEL_MASK;
			return usartGetStatus();
		case DRV_CTRL_SETBIT:
		case DRV_CTRL_CLEARBIT:
		case DRV_CTRL_GETBIT:
			if( data == 0 ) { return usartSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return usartSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			if( command == DRV_CTRL_SETBIT ) { TM_SETBIT(usart_status, data->status_bit); }
			if( command == DRV_CTRL_CLEARBIT ) { TM_CLEARBIT(usart_status, data->status_bit); }
			if( command == DRV_CTRL_GETBIT )
			{
				data->bit_value = TM_GETBIT(usart_status, data->status_bit) != 0;
			}
			return usartGetStatus();
		case DRV_CTRL_GETSTATUS:
			return usartGetStatus();
		case DRV_CTRL_GETLASTERROR:
			if( data == 0 ) { return usartSetError(ERR_NULL_POINTER); }
			data->error = usart_last_error;
			return usartGetStatus();
		default:
			return usartSetError(ERR_HAL_DRIVER_INVALID_CONTROL);
	}
}
