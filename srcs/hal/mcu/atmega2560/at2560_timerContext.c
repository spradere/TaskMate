/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file at2560_timerContext.c
 * @brief ATmega2560 context-switch timer implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <avr/io.h>
#include <stddef.h>

#include "interfaces/drv_timerContext.h"
#include "interfaces/tm_macros.h"
#include "interfaces/tm_runLevel.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define AT2560TIMERCONTEXT_COUNT_OVERFLOW 1999U // Interrupt every 1 ms
#define AT2560TIMERCONTEXT_COUNT_GUARD 4U

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

hal_timerContextCallback_ptr_t schedCallback = NULL;
static hal_driver_status_t timer_context_status;
static tm_run_level_t timer_context_run_level = RL_RUN_NONE;
static err_codes_t timer_context_last_error = ERR_NO_ERROR;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

void asm_timerContextClockStart(void);
void asm_timerContextClockStop(void);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Driver state
 * ---------------------------------------------*/

static hal_driver_state_t timerContextSetError(err_codes_t error)
{
	timer_context_last_error = error;
	return DRV_STATE_ERROR;
}

static hal_driver_state_t hal_timerContextGetStatus(void)
{
	if( TM_GETBIT(timer_context_status, DRV_BIT_DEAD) != 0U )
	{
		timer_context_last_error = ERR_HAL_DRIVER_DEAD;
		return DRV_STATE_DEAD;
	}
	if( TM_GETBIT(timer_context_status, DRV_BIT_ERROR) != 0U )
	{
		return timerContextSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( TM_GETBIT(timer_context_status, DRV_BIT_INIT) == 0U )
	{
		if( TM_GETBIT(timer_context_status, DRV_BIT_START) == 0U ) { return DRV_STATE_OFF; }
		return timerContextSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( TM_GETBIT(timer_context_status, DRV_BIT_START) == 0U ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t timerContextRequireRunning(void)
{
	hal_driver_state_t state = hal_timerContextGetStatus();
	if( (state == DRV_STATE_OFF) || (state == DRV_STATE_INITIALIZED) )
	{
		return timerContextSetError(ERR_HAL_DRIVER_NOT_RUNNING);
	}
	return state;
}

/* -----------------------------------------------
 * Callback and scheduler trigger
 * ---------------------------------------------*/

hal_driver_state_t hal_timerContextSetCallback(hal_timerContextCallback_ptr_t func_ptr)
{
	if( func_ptr == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
	schedCallback = func_ptr;
	return hal_timerContextGetStatus();
}

hal_driver_state_t hal_timerContextLoad(void)
{
	hal_driver_state_t state = timerContextRequireRunning();
	if( state != DRV_STATE_RUNNING ) { return state; }
	const uint16_t LOAD = AT2560TIMERCONTEXT_COUNT_OVERFLOW - AT2560TIMERCONTEXT_COUNT_GUARD;

	TCNT1 = LOAD;
	return DRV_STATE_RUNNING;
}

/* -----------------------------------------------
 * Driver life cycle
 * ---------------------------------------------*/

static hal_driver_state_t hal_timerContextInit(void)
{
	if( TM_GETBIT(timer_context_status, DRV_BIT_DEAD) != 0U )
	{
		return timerContextSetError(ERR_HAL_DRIVER_DEAD);
	}
	// Configure CTC mode while the clock is stopped, then load TOP from a known count.
	TCCR1A = 0U;
	TM_WRITEBIT(TCCR1B, WGM12);
	TCNT1 = 0U;
	OCR1A = AT2560TIMERCONTEXT_COUNT_OVERFLOW;
	TM_CLEARBIT(TIMSK1, OCIE1A);
	TM_WRITEBIT(TIFR1, OCF1A);

	TM_SETBIT(timer_context_status, DRV_BIT_INIT);
	timer_context_last_error = ERR_NO_ERROR;
	return DRV_STATE_INITIALIZED;
}

static hal_driver_state_t hal_timerContextStart(void)
{
	if( TM_GETBIT(timer_context_status, DRV_BIT_DEAD) != 0U )
	{
		return timerContextSetError(ERR_HAL_DRIVER_DEAD);
	}
	if( TM_GETBIT(timer_context_status, DRV_BIT_INIT) == 0U )
	{

		return timerContextSetError(ERR_HAL_DRIVER_NOT_INITIALIZED);
	}

	// Clear a stale compare before enabling the interrupt and the divide-by-8 clock.
	TCNT1 = 0U;
	TM_WRITEBIT(TIFR1, OCF1A);
	TM_SETBIT(TIMSK1, OCIE1A);
	asm_timerContextClockStart();
	TM_SETBIT(timer_context_status, DRV_BIT_START);
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t hal_timerContextStop(void)
{
	asm_timerContextClockStop();
	TM_CLEARBIT(TIMSK1, OCIE1A);
	TM_WRITEBIT(TIFR1, OCF1A);
	TM_CLEARBIT(timer_context_status, DRV_BIT_START);
	return hal_timerContextGetStatus();
}

/* -----------------------------------------------
 * Driver control
 * ---------------------------------------------*/

hal_driver_state_t hal_timerContextControl(hal_driver_control_t command,
										   hal_driver_control_data_t *data)
{
	switch( command )
	{
		case DRV_CTRL_INIT:
			return hal_timerContextInit();
		case DRV_CTRL_START:
			return hal_timerContextStart();
		case DRV_CTRL_STOP:
			return hal_timerContextStop();
		// Keep lifecycle flags when updating the run-level bits.
		case DRV_CTRL_RLSET:
			if( data == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
			if( data->run_level >= RL_LEVEL_COUNT )
			{
				return timerContextSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			timer_context_run_level = data->run_level;
			return hal_timerContextGetStatus();
		case DRV_CTRL_RLGET:
			if( data == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
			data->run_level = timer_context_run_level;
			return hal_timerContextGetStatus();
		// Limit bit operations to the shared driver status flags.
		case DRV_CTRL_SETBIT:
			if( data == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
			if( data->status_bit > DRV_BIT_DEAD )
			{
				return timerContextSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			TM_SETBIT(timer_context_status, data->status_bit);
			return hal_timerContextGetStatus();
		case DRV_CTRL_CLEARBIT:
			if( data == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
			if( data->status_bit > DRV_BIT_DEAD )
			{
				return timerContextSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			TM_CLEARBIT(timer_context_status, data->status_bit);
			return hal_timerContextGetStatus();
		case DRV_CTRL_GETBIT:
			if( data == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
			if( data->status_bit > DRV_BIT_DEAD )
			{
				return timerContextSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			data->bit_value = TM_GETBIT(timer_context_status, data->status_bit) != 0U;
			return hal_timerContextGetStatus();
		// Expose current state and the most recent driver error separately.
		case DRV_CTRL_GETSTATUS:
			return hal_timerContextGetStatus();
		case DRV_CTRL_GETLASTERROR:
			if( data == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
			data->error = timer_context_last_error;
			return hal_timerContextGetStatus();
		default:
			return timerContextSetError(ERR_HAL_DRIVER_INVALID_CONTROL);
	}
}
