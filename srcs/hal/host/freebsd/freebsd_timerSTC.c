/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file freebsd_timerSTC.c
 * @brief FreeBSD virtual software-counter timer implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "interfaces/drv_timerSTC.h"

#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

#include "freebsd_interrupts.h"
#include "interfaces/tm_macros.h"
#include "interfaces/tm_runLevel.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define FREEBSD_STC_INTERVAL_US 10000

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static hal_timerSTCCallback_t stc_callback;
static hal_driver_status_t timer_stc_status;
static err_codes_t timer_stc_last_error = ERR_NO_ERROR;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void timerSTCHandler(int signal, siginfo_t *info, void *native_context);
static hal_driver_state_t timerSTCSetError(err_codes_t error);
static hal_driver_state_t timerSTCGetStatus(void);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static hal_driver_state_t timerSTCSetError(err_codes_t error)
{
	timer_stc_last_error = error;
	return DRV_STATE_ERROR;
}

static hal_driver_state_t timerSTCGetStatus(void)
{
	if( TM_GETBIT(timer_stc_status, DRV_BIT_DEAD) != 0 )
	{
		timer_stc_last_error = ERR_HAL_DRIVER_DEAD;
		return DRV_STATE_DEAD;
	}
	if( TM_GETBIT(timer_stc_status, DRV_BIT_ERROR) != 0 )
	{
		return timerSTCSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( TM_GETBIT(timer_stc_status, DRV_BIT_INIT) == 0 ) { return DRV_STATE_OFF; }
	if( TM_GETBIT(timer_stc_status, DRV_BIT_START) == 0 ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

static void timerSTCHandler(int signal, siginfo_t *info, void *native_context)
{
	(void)info;
	(void)native_context;
	if( signal != FREEBSD_SIGNAL_STC ) { return; }
	if( stc_callback != 0 ) { stc_callback(); }
}

hal_driver_state_t hal_timerSTCSetCallback(hal_timerSTCCallback_t func_ptr)
{
	if( func_ptr == 0 ) { return timerSTCSetError(ERR_NULL_POINTER); }
	stc_callback = func_ptr;
	return timerSTCGetStatus();
}

static hal_driver_state_t timerSTCInit(void)
{
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	action.sa_sigaction = timerSTCHandler;
	action.sa_flags = SA_SIGINFO;
	if( !freebsd_interruptsMask(&action.sa_mask) )
	{
		return timerSTCSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( sigaction(FREEBSD_SIGNAL_STC, &action, 0) != 0 )
	{
		return timerSTCSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}

	TM_SETBIT(timer_stc_status, DRV_BIT_INIT);
	timer_stc_last_error = ERR_NO_ERROR;
	return DRV_STATE_INITIALIZED;
}

static hal_driver_state_t timerSTCStart(void)
{
	if( TM_GETBIT(timer_stc_status, DRV_BIT_INIT) == 0 )
	{
		return timerSTCSetError(ERR_HAL_DRIVER_NOT_INITIALIZED);
	}

	const struct itimerval timer = {
		.it_interval = {.tv_sec = 0, .tv_usec = FREEBSD_STC_INTERVAL_US},
		.it_value = {.tv_sec = 0, .tv_usec = FREEBSD_STC_INTERVAL_US},
	};
	if( setitimer(ITIMER_REAL, &timer, 0) != 0 )
	{
		return timerSTCSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}

	TM_SETBIT(timer_stc_status, DRV_BIT_START);
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t timerSTCStop(void)
{
	const struct itimerval timer = {0};
	if( setitimer(ITIMER_REAL, &timer, 0) != 0 )
	{
		return timerSTCSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	TM_CLEARBIT(timer_stc_status, DRV_BIT_START);
	return timerSTCGetStatus();
}

hal_driver_state_t hal_timerSTCControl(hal_driver_control_t command,
									   hal_driver_control_data_t *data)
{
	switch( command )
	{
		case DRV_CTRL_INIT:
			return timerSTCInit();
		case DRV_CTRL_START:
			return timerSTCStart();
		case DRV_CTRL_STOP:
			return timerSTCStop();
		case DRV_CTRL_RLSET:
			if( data == 0 ) { return timerSTCSetError(ERR_NULL_POINTER); }
			if( data->run_level >= RL_LEVEL_COUNT )
			{
				return timerSTCSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			timer_stc_status &= (hal_driver_status_t)~RL_LEVEL_MASK;
			timer_stc_status |= data->run_level;
			return timerSTCGetStatus();
		case DRV_CTRL_RLGET:
			if( data == 0 ) { return timerSTCSetError(ERR_NULL_POINTER); }
			data->run_level = timer_stc_status & RL_LEVEL_MASK;
			return timerSTCGetStatus();
		case DRV_CTRL_SETBIT:
		case DRV_CTRL_CLEARBIT:
		case DRV_CTRL_GETBIT:
			if( data == 0 ) { return timerSTCSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return timerSTCSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			if( command == DRV_CTRL_SETBIT ) { TM_SETBIT(timer_stc_status, data->status_bit); }
			if( command == DRV_CTRL_CLEARBIT )
			{
				TM_CLEARBIT(timer_stc_status, data->status_bit);
			}
			if( command == DRV_CTRL_GETBIT )
			{
				data->bit_value = TM_GETBIT(timer_stc_status, data->status_bit) != 0;
			}
			return timerSTCGetStatus();
		case DRV_CTRL_GETSTATUS:
			return timerSTCGetStatus();
		case DRV_CTRL_GETLASTERROR:
			if( data == 0 ) { return timerSTCSetError(ERR_NULL_POINTER); }
			data->error = timer_stc_last_error;
			return timerSTCGetStatus();
		default:
			return timerSTCSetError(ERR_HAL_DRIVER_INVALID_CONTROL);
	}
}
