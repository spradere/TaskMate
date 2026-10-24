/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file at2560_timerMonotonic.c
 * @brief ATmega2560 Timer3 monotonic tick implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "interfaces/drv_timerMonotonic.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>

#include "interfaces/hal_atomic.h"
#include "interfaces/tm_macros.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define AT2560TIMERMONOTONIC_FREQ_TICKS_Hz 10000UL // One tick every 100 us.
#define AT2560TIMERMONOTONIC_DIV_CLOCK 8UL
#define AT2560TIMERMONOTONIC_COUNT_TOP \
	(F_CPU / (AT2560TIMERMONOTONIC_DIV_CLOCK * AT2560TIMERMONOTONIC_FREQ_TICKS_Hz) - 1UL)

_Static_assert(F_CPU % (AT2560TIMERMONOTONIC_DIV_CLOCK * AT2560TIMERMONOTONIC_FREQ_TICKS_Hz) == 0UL,
			   "Timer3 cannot generate an exact 100 us period");
_Static_assert(AT2560TIMERMONOTONIC_COUNT_TOP <= UINT16_MAX, "Timer3 compare value overflows");

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static volatile uint32_t timer_monotonic_ticks;
static hal_driver_status_t timer_monotonic_status;
static tm_run_level_t timer_monotonic_run_level = RL_RUN_NONE;
static err_codes_t timer_monotonic_last_error = ERR_NO_ERROR;

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Driver state
 * ---------------------------------------------*/

static hal_driver_state_t timerMonotonicSetError(err_codes_t error)
{
	timer_monotonic_last_error = error;
	return DRV_STATE_ERROR;
}

static hal_driver_state_t timerMonotonicGetStatus(void)
{
	if( TM_GETBIT(timer_monotonic_status, DRV_BIT_DEAD) != 0U )
	{
		timer_monotonic_last_error = ERR_HAL_DRIVER_DEAD;
		return DRV_STATE_DEAD;
	}
	if( TM_GETBIT(timer_monotonic_status, DRV_BIT_ERROR) != 0U )
	{
		return timerMonotonicSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( TM_GETBIT(timer_monotonic_status, DRV_BIT_INIT) == 0U )
	{
		if( TM_GETBIT(timer_monotonic_status, DRV_BIT_START) == 0U ) { return DRV_STATE_OFF; }
		return timerMonotonicSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( TM_GETBIT(timer_monotonic_status, DRV_BIT_START) == 0U ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

/* -----------------------------------------------
 * Timer and life cycle
 * ---------------------------------------------*/

ISR(TIMER3_COMPA_vect) { timer_monotonic_ticks++; }

uint32_t hal_timerMonotonicRead(void)
{
	hal_atomic_state_t state = hal_atomicStart();
	uint32_t ticks = timer_monotonic_ticks;
	hal_atomicEnd(state);
	return ticks;
}

static hal_driver_state_t timerMonotonicInit(void)
{
	if( TM_GETBIT(timer_monotonic_status, DRV_BIT_DEAD) != 0U )
	{
		return timerMonotonicSetError(ERR_HAL_DRIVER_DEAD);
	}
	// Configure Timer3 CTC with a stopped clock and a known compare value.
	TCCR3A = 0U;
	TCCR3B = TM_BIT_MASK_8(WGM32);
	TCNT3 = 0U;
	OCR3A = (uint16_t)AT2560TIMERMONOTONIC_COUNT_TOP;
	TM_CLEARBIT(TIMSK3, OCIE3A);
	TM_WRITEBIT(TIFR3, OCF3A);

	TM_SETBIT(timer_monotonic_status, DRV_BIT_INIT);
	timer_monotonic_last_error = ERR_NO_ERROR;
	return DRV_STATE_INITIALIZED;
}

static hal_driver_state_t timerMonotonicStart(void)
{
	if( TM_GETBIT(timer_monotonic_status, DRV_BIT_DEAD) != 0U )
	{
		return timerMonotonicSetError(ERR_HAL_DRIVER_DEAD);
	}
	if( TM_GETBIT(timer_monotonic_status, DRV_BIT_INIT) == 0U )
	{
		return timerMonotonicSetError(ERR_HAL_DRIVER_NOT_INITIALIZED);
	}

	TCNT3 = 0U;
	TM_WRITEBIT(TIFR3, OCF3A);
	TM_SETBIT(TIMSK3, OCIE3A);
	TM_SETBIT(TCCR3B, CS31);
	TM_SETBIT(timer_monotonic_status, DRV_BIT_START);
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t timerMonotonicStop(void)
{
	TM_CLEARBIT(TCCR3B, CS31);
	TM_CLEARBIT(TIMSK3, OCIE3A);
	TM_WRITEBIT(TIFR3, OCF3A);
	TM_CLEARBIT(timer_monotonic_status, DRV_BIT_START);
	return timerMonotonicGetStatus();
}

/* -----------------------------------------------
 * Driver control
 * ---------------------------------------------*/

hal_driver_state_t hal_timerMonotonicControl(hal_driver_control_t command,
											 hal_driver_control_data_t *data)
{
	switch( command )
	{
		case DRV_CTRL_INIT:
			return timerMonotonicInit();
		case DRV_CTRL_START:
			return timerMonotonicStart();
		case DRV_CTRL_STOP:
			return timerMonotonicStop();
		case DRV_CTRL_RLSET:
			if( data == NULL ) { return timerMonotonicSetError(ERR_NULL_POINTER); }
			if( data->run_level >= RL_LEVEL_COUNT )
			{
				return timerMonotonicSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			timer_monotonic_run_level = data->run_level;
			return timerMonotonicGetStatus();
		case DRV_CTRL_RLGET:
			if( data == NULL ) { return timerMonotonicSetError(ERR_NULL_POINTER); }
			data->run_level = timer_monotonic_run_level;
			return timerMonotonicGetStatus();
		case DRV_CTRL_SETBIT:
			if( data == NULL ) { return timerMonotonicSetError(ERR_NULL_POINTER); }
			if( data->status_bit > DRV_BIT_DEAD )
			{
				return timerMonotonicSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			TM_SETBIT(timer_monotonic_status, data->status_bit);
			return timerMonotonicGetStatus();
		case DRV_CTRL_CLEARBIT:
			if( data == NULL ) { return timerMonotonicSetError(ERR_NULL_POINTER); }
			if( data->status_bit > DRV_BIT_DEAD )
			{
				return timerMonotonicSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			TM_CLEARBIT(timer_monotonic_status, data->status_bit);
			return timerMonotonicGetStatus();
		case DRV_CTRL_GETBIT:
			if( data == NULL ) { return timerMonotonicSetError(ERR_NULL_POINTER); }
			if( data->status_bit > DRV_BIT_DEAD )
			{
				return timerMonotonicSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			data->bit_value = TM_GETBIT(timer_monotonic_status, data->status_bit) != 0U;
			return timerMonotonicGetStatus();
		case DRV_CTRL_GETSTATUS:
			return timerMonotonicGetStatus();
		case DRV_CTRL_GETLASTERROR:
			if( data == NULL ) { return timerMonotonicSetError(ERR_NULL_POINTER); }
			data->error = timer_monotonic_last_error;
			return timerMonotonicGetStatus();
		default:
			return timerMonotonicSetError(ERR_HAL_DRIVER_INVALID_CONTROL);
	}
}
