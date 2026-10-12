/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file freebsd_usart.cpp
 * @brief FreeBSD virtual USART implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "interfaces/drv_usart.h"

#include <stddef.h>
#include <stdint.h>

#include "hal/board/pc/pc_console.h"
#include "interfaces/hal_atomic.h"
#include "interfaces/tm_runLevel.h"

/* -----------------------------------------------
 * Constants and private types
 * ---------------------------------------------*/

static constexpr uint16_t FREEBSDUSART_SIZE_TXBUFFER = 256U;

enum class status_bit_t : uint8_t
{
	init = DRV_BIT_INIT,
	start = DRV_BIT_START,
	error = DRV_BIT_ERROR,
	dead = DRV_BIT_DEAD
};

enum class run_level_t : uint8_t
{
	none = RL_RUN_NONE,
	core = RL_RUN_CORE,
	driver = RL_RUN_DRIVER,
	service = RL_RUN_SERVICE,
	user = RL_RUN_USER
};

static constexpr hal_driver_status_t FREEBSDUSART_MASK_INIT =
	static_cast<hal_driver_status_t>(1U << static_cast<uint8_t>(status_bit_t::init));
static constexpr hal_driver_status_t FREEBSDUSART_MASK_START =
	static_cast<hal_driver_status_t>(1U << static_cast<uint8_t>(status_bit_t::start));
static constexpr hal_driver_status_t FREEBSDUSART_MASK_ERROR =
	static_cast<hal_driver_status_t>(1U << static_cast<uint8_t>(status_bit_t::error));
static constexpr hal_driver_status_t FREEBSDUSART_MASK_DEAD =
	static_cast<hal_driver_status_t>(1U << static_cast<uint8_t>(status_bit_t::dead));

struct usart_context_t
{
	uint8_t tx_buffer[FREEBSDUSART_SIZE_TXBUFFER];
	uint16_t tx_length;
	hal_driver_status_t status;
};

static_assert(RL_LEVEL_COUNT <= (RL_LEVEL_MASK + 1U), "Run levels exceed the status field");

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static constinit usart_context_t usart_context = {};
static constinit err_codes_t usart_last_error = ERR_NO_ERROR;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static hal_driver_state_t usartSetError(err_codes_t error);
static hal_driver_state_t usartGetStatus(usart_context_t &context);
static hal_driver_state_t usartRequireRunning(usart_context_t &context);
static hal_driver_state_t usartControlCore(usart_context_t &context,
										   hal_driver_control_t command,
										   hal_driver_control_data_t *data);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static hal_driver_state_t usartSetError(err_codes_t error)
{
	usart_last_error = error;
	return DRV_STATE_ERROR;
}

static hal_driver_state_t usartGetStatus(usart_context_t &context)
{
	if( (context.status & FREEBSDUSART_MASK_DEAD) != 0U )
	{
		usart_last_error = ERR_HAL_DRIVER_DEAD;
		return DRV_STATE_DEAD;
	}
	if( (context.status & FREEBSDUSART_MASK_ERROR) != 0U )
	{
		return usartSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( (context.status & FREEBSDUSART_MASK_INIT) == 0U ) { return DRV_STATE_OFF; }
	if( (context.status & FREEBSDUSART_MASK_START) == 0U )
	{
		return DRV_STATE_INITIALIZED;
	}
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t usartRequireRunning(usart_context_t &context)
{
	hal_driver_state_t state = usartGetStatus(context);
	if( (state == DRV_STATE_OFF) || (state == DRV_STATE_INITIALIZED) )
	{
		return usartSetError(ERR_HAL_DRIVER_NOT_RUNNING);
	}
	return state;
}

static hal_driver_state_t usartInit(usart_context_t &context)
{
	if( !pc_consoleInit() ) { return usartSetError(ERR_HAL_DRIVER_DEPENDENCY); }
	context.tx_length = 0U;
	context.status |= FREEBSDUSART_MASK_INIT;
	usart_last_error = ERR_NO_ERROR;
	return DRV_STATE_INITIALIZED;
}

static hal_driver_state_t usartStart(usart_context_t &context)
{
	if( (context.status & FREEBSDUSART_MASK_INIT) == 0U )
	{
		return usartSetError(ERR_HAL_DRIVER_NOT_INITIALIZED);
	}
	context.status |= FREEBSDUSART_MASK_START;
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t usartStop(usart_context_t &context)
{
	context.status &= static_cast<hal_driver_status_t>(~FREEBSDUSART_MASK_START);
	return usartGetStatus(context);
}

hal_driver_state_t hal_usartRead(uint8_t *data)
{
	hal_driver_state_t state = usartRequireRunning(usart_context);
	if( state != DRV_STATE_RUNNING ) { return state; }
	if( data == NULL ) { return usartSetError(ERR_NULL_POINTER); }

	hal_atomic_state_t atomic_state = hal_atomicStart();
	bool have_data = pc_consoleReadByte(data);
	hal_atomicEnd(atomic_state);
	if( !have_data ) { return usartSetError(ERR_HAL_USART_RX_BUFFER_EMPTY); }
	return DRV_STATE_RUNNING;
}

hal_driver_state_t hal_usartWriteByte(uint8_t data)
{
	hal_driver_state_t state = usartRequireRunning(usart_context);
	if( state != DRV_STATE_RUNNING ) { return state; }
	if( usart_context.tx_length >= FREEBSDUSART_SIZE_TXBUFFER )
	{
		return usartSetError(ERR_HAL_USART_TX_BUFFER_FULL);
	}
	usart_context.tx_buffer[usart_context.tx_length++] = data;
	return DRV_STATE_RUNNING;
}

hal_driver_state_t hal_usartSendTXBuffer(void)
{
	hal_driver_state_t state = usartRequireRunning(usart_context);
	if( state != DRV_STATE_RUNNING ) { return state; }

	hal_atomic_state_t atomic_state = hal_atomicStart();
	for( uint16_t i = 0U; i < usart_context.tx_length; i++ )
	{
		pc_consoleWriteByte(usart_context.tx_buffer[i]);
	}
	pc_consoleFlush();
	usart_context.tx_length = 0U;
	hal_atomicEnd(atomic_state);
	return DRV_STATE_RUNNING;
}

/* Keep the complete common control protocol in one non-template body. */
static hal_driver_state_t usartControlCore(usart_context_t &context,
										   hal_driver_control_t command,
										   hal_driver_control_data_t *data)
{
	switch( command )
	{
		case DRV_CTRL_INIT:
			return usartInit(context);
		case DRV_CTRL_START:
			return usartStart(context);
		case DRV_CTRL_STOP:
			return usartStop(context);
		// Keep lifecycle flags when updating the run-level bits.
		case DRV_CTRL_RLSET:
			if( data == NULL ) { return usartSetError(ERR_NULL_POINTER); }
			if( data->run_level >= RL_LEVEL_COUNT )
			{
				return usartSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			context.status &= static_cast<hal_driver_status_t>(~RL_LEVEL_MASK);
			context.status |= static_cast<hal_driver_status_t>(
				static_cast<run_level_t>(data->run_level));
			return usartGetStatus(context);
		case DRV_CTRL_RLGET:
			if( data == NULL ) { return usartSetError(ERR_NULL_POINTER); }
			data->run_level = static_cast<tm_run_level_t>(
				static_cast<run_level_t>(context.status & RL_LEVEL_MASK));
			return usartGetStatus(context);
		// Limit bit operations to the shared driver status flags.
		case DRV_CTRL_SETBIT:
		case DRV_CTRL_CLEARBIT:
		case DRV_CTRL_GETBIT:
			if( data == NULL ) { return usartSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return usartSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			{
				const auto bit = static_cast<status_bit_t>(data->status_bit);
				const auto mask = static_cast<hal_driver_status_t>(
					1U << static_cast<uint8_t>(bit));
				if( command == DRV_CTRL_SETBIT ) { context.status |= mask; }
				if( command == DRV_CTRL_CLEARBIT )
				{
					context.status &= static_cast<hal_driver_status_t>(~mask);
				}
				if( command == DRV_CTRL_GETBIT )
				{
					data->bit_value = (context.status & mask) != 0U;
				}
			}
			return usartGetStatus(context);
		// Expose current state and the most recent driver error separately.
		case DRV_CTRL_GETSTATUS:
			return usartGetStatus(context);
		case DRV_CTRL_GETLASTERROR:
			if( data == NULL ) { return usartSetError(ERR_NULL_POINTER); }
			data->error = usart_last_error;
			return usartGetStatus(context);
		default:
			return usartSetError(ERR_HAL_DRIVER_INVALID_CONTROL);
	}
}

hal_driver_state_t hal_usartControl(hal_driver_control_t command, hal_driver_control_data_t *data)
{
	return usartControlCore(usart_context, command, data);
}
