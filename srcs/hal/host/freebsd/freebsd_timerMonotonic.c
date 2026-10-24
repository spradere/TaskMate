/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file freebsd_timerMonotonic.c
 * @brief FreeBSD monotonic timer implementation with 100 us ticks.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "interfaces/drv_timerMonotonic.h"

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include "interfaces/hal_atomic.h"
#include "interfaces/hal_halt.h"
#include "interfaces/tm_macros.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define FREEBSDTIMERMONOTONIC_NS_PER_SECOND 1000000000LL
#define FREEBSDTIMERMONOTONIC_NS_PER_TICK 100000LL

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static uint32_t timer_monotonic_ticks;
static struct timespec timer_monotonic_started_at;
static hal_driver_status_t timer_monotonic_status;
static tm_run_level_t timer_monotonic_run_level = RL_RUN_NONE;
static err_codes_t timer_monotonic_last_error = ERR_NO_ERROR;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static hal_driver_state_t timerMonotonicSetError(err_codes_t error);
static hal_driver_state_t timerMonotonicGetStatus(void);
static uint32_t timerMonotonicTicksAt(const struct timespec *now);
static hal_driver_state_t timerMonotonicControlLocked(hal_driver_control_t command,
													  hal_driver_control_data_t *data);

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

static uint32_t timerMonotonicTicksAt(const struct timespec *now)
{
	const int64_t elapsed_ns =
		((int64_t)now->tv_sec - (int64_t)timer_monotonic_started_at.tv_sec) *
			FREEBSDTIMERMONOTONIC_NS_PER_SECOND +
		(int64_t)now->tv_nsec - (int64_t)timer_monotonic_started_at.tv_nsec;
	if( elapsed_ns < 0 ) { hal_halt(); }
	return timer_monotonic_ticks + (uint32_t)(elapsed_ns / FREEBSDTIMERMONOTONIC_NS_PER_TICK);
}

uint32_t hal_timerMonotonicRead(void)
{
	hal_atomic_state_t state = hal_atomicStart();
	uint32_t ticks = timer_monotonic_ticks;
	if( TM_GETBIT(timer_monotonic_status, DRV_BIT_START) != 0U )
	{
		struct timespec now;
		if( clock_gettime(CLOCK_MONOTONIC, &now) != 0U ) { hal_halt(); }
		ticks = timerMonotonicTicksAt(&now);
	}
	hal_atomicEnd(state);
	return ticks;
}

static hal_driver_state_t timerMonotonicInit(void)
{
	if( TM_GETBIT(timer_monotonic_status, DRV_BIT_DEAD) != 0U )
	{
		return timerMonotonicSetError(ERR_HAL_DRIVER_DEAD);
	}
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
	if( TM_GETBIT(timer_monotonic_status, DRV_BIT_START) != 0U ) { return DRV_STATE_RUNNING; }
	if( clock_gettime(CLOCK_MONOTONIC, &timer_monotonic_started_at) != 0U )
	{
		return timerMonotonicSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	TM_SETBIT(timer_monotonic_status, DRV_BIT_START);
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t timerMonotonicStop(void)
{
	if( TM_GETBIT(timer_monotonic_status, DRV_BIT_START) != 0U )
	{
		struct timespec now;
		if( clock_gettime(CLOCK_MONOTONIC, &now) != 0U )
		{
			return timerMonotonicSetError(ERR_HAL_DRIVER_INVALID_STATE);
		}
		timer_monotonic_ticks = timerMonotonicTicksAt(&now);
	}
	TM_CLEARBIT(timer_monotonic_status, DRV_BIT_START);
	return timerMonotonicGetStatus();
}

/* -----------------------------------------------
 * Driver control
 * ---------------------------------------------*/

static hal_driver_state_t timerMonotonicControlLocked(hal_driver_control_t command,
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

hal_driver_state_t hal_timerMonotonicControl(hal_driver_control_t command,
											 hal_driver_control_data_t *data)
{
	hal_atomic_state_t state = hal_atomicStart();
	hal_driver_state_t result = timerMonotonicControlLocked(command, data);
	hal_atomicEnd(state);
	return result;
}
