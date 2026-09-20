/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file freebsd_timerSched.c
 * @brief FreeBSD virtual scheduler-timer implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "freebsd_timerSched.h"

#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <ucontext.h>

#include "freebsd_interrupts.h"
#include "hal/target/ucontext/ucontext_types.h"
#include "interfaces/drv_timerSched.h"
#include "interfaces/hal_halt.h"
#include "interfaces/tm_macros.h"
#include "interfaces/tm_runLevel.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define FREEBSD_SCHED_INTERVAL_US 1000

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static hal_timerSchedCallback_ptr_t sched_callback;
static hal_driver_status_t timer_sched_status;
static err_codes_t timer_sched_last_error = ERR_NO_ERROR;
static bool timer_sched_activated;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void timerSchedHandler(int signal, siginfo_t *info, void *native_context);
static hal_driver_state_t timerSchedSetError(err_codes_t error);
static hal_driver_state_t timerSchedGetStatus(void);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static hal_driver_state_t timerSchedSetError(err_codes_t error)
{
	timer_sched_last_error = error;
	return DRV_STATE_ERROR;
}

static hal_driver_state_t timerSchedGetStatus(void)
{
	if( TM_GETBIT(timer_sched_status, DRV_BIT_DEAD) != 0 )
	{
		timer_sched_last_error = ERR_HAL_DRIVER_DEAD;
		return DRV_STATE_DEAD;
	}
	if( TM_GETBIT(timer_sched_status, DRV_BIT_ERROR) != 0 )
	{
		return timerSchedSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( TM_GETBIT(timer_sched_status, DRV_BIT_INIT) == 0 ) { return DRV_STATE_OFF; }
	if( TM_GETBIT(timer_sched_status, DRV_BIT_START) == 0 ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

static void timerSchedHandler(int signal, siginfo_t *info, void *native_context)
{
	(void)info;
	if( (signal != FREEBSD_SIGNAL_SCHED) || (sched_callback == 0) ||
		(native_context == 0) )
	{
		hal_halt();
	}

	hal_context_t interrupted_context = {.native = (ucontext_t *)native_context};
	hal_context_t *next_context = sched_callback(&interrupted_context);
	if( next_context == 0 ) { hal_halt(); }

	/* FreeBSD restores registers, stack, and signal mask atomically here. */
	if( sigreturn(next_context->native) != 0 ) { hal_halt(); }
	hal_halt();
}

hal_driver_state_t hal_timerSchedSetCallback(hal_timerSchedCallback_ptr_t func_ptr)
{
	if( func_ptr == 0 ) { return timerSchedSetError(ERR_NULL_POINTER); }
	sched_callback = func_ptr;
	return timerSchedGetStatus();
}

hal_driver_state_t hal_timerSchedLoad(void)
{
	if( timerSchedGetStatus() != DRV_STATE_RUNNING )
	{
		return timerSchedSetError(ERR_HAL_DRIVER_NOT_RUNNING);
	}
	if( raise(FREEBSD_SIGNAL_SCHED) != 0 )
	{
		return timerSchedSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	return DRV_STATE_RUNNING;
}

void freebsd_timerSchedActivate(void)
{
	if( timer_sched_activated ) { return; }
	if( timerSchedGetStatus() != DRV_STATE_RUNNING ) { hal_halt(); }

	const struct itimerval timer = {
		.it_interval = {.tv_sec = 0, .tv_usec = FREEBSD_SCHED_INTERVAL_US},
		.it_value = {.tv_sec = 0, .tv_usec = FREEBSD_SCHED_INTERVAL_US},
	};
	if( setitimer(ITIMER_VIRTUAL, &timer, 0) != 0 ) { hal_halt(); }
	timer_sched_activated = true;
}

static hal_driver_state_t timerSchedInit(void)
{
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	action.sa_sigaction = timerSchedHandler;
	action.sa_flags = SA_SIGINFO;
	if( !freebsd_interruptsMask(&action.sa_mask) )
	{
		return timerSchedSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( sigaction(FREEBSD_SIGNAL_SCHED, &action, 0) != 0 )
	{
		return timerSchedSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}

	TM_SETBIT(timer_sched_status, DRV_BIT_INIT);
	timer_sched_last_error = ERR_NO_ERROR;
	return DRV_STATE_INITIALIZED;
}

static hal_driver_state_t timerSchedStart(void)
{
	if( TM_GETBIT(timer_sched_status, DRV_BIT_INIT) == 0 )
	{
		return timerSchedSetError(ERR_HAL_DRIVER_NOT_INITIALIZED);
	}
	TM_SETBIT(timer_sched_status, DRV_BIT_START);
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t timerSchedStop(void)
{
	const struct itimerval timer = {0};
	if( setitimer(ITIMER_VIRTUAL, &timer, 0) != 0 )
	{
		return timerSchedSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	timer_sched_activated = false;
	TM_CLEARBIT(timer_sched_status, DRV_BIT_START);
	return timerSchedGetStatus();
}

hal_driver_state_t hal_timerSchedControl(hal_driver_control_t command,
										 hal_driver_control_data_t *data)
{
	switch( command )
	{
		case DRV_CTRL_INIT:
			return timerSchedInit();
		case DRV_CTRL_START:
			return timerSchedStart();
		case DRV_CTRL_STOP:
			return timerSchedStop();
		case DRV_CTRL_RLSET:
			if( data == 0 ) { return timerSchedSetError(ERR_NULL_POINTER); }
			if( data->run_level >= RL_LEVEL_COUNT )
			{
				return timerSchedSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			timer_sched_status &= (hal_driver_status_t)~RL_LEVEL_MASK;
			timer_sched_status |= data->run_level;
			return timerSchedGetStatus();
		case DRV_CTRL_RLGET:
			if( data == 0 ) { return timerSchedSetError(ERR_NULL_POINTER); }
			data->run_level = timer_sched_status & RL_LEVEL_MASK;
			return timerSchedGetStatus();
		case DRV_CTRL_SETBIT:
		case DRV_CTRL_CLEARBIT:
		case DRV_CTRL_GETBIT:
			if( data == 0 ) { return timerSchedSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return timerSchedSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			if( command == DRV_CTRL_SETBIT )
			{
				TM_SETBIT(timer_sched_status, data->status_bit);
			}
			if( command == DRV_CTRL_CLEARBIT )
			{
				TM_CLEARBIT(timer_sched_status, data->status_bit);
			}
			if( command == DRV_CTRL_GETBIT )
			{
				data->bit_value = TM_GETBIT(timer_sched_status, data->status_bit) != 0;
			}
			return timerSchedGetStatus();
		case DRV_CTRL_GETSTATUS:
			return timerSchedGetStatus();
		case DRV_CTRL_GETLASTERROR:
			if( data == 0 ) { return timerSchedSetError(ERR_NULL_POINTER); }
			data->error = timer_sched_last_error;
			return timerSchedGetStatus();
		default:
			return timerSchedSetError(ERR_HAL_DRIVER_INVALID_CONTROL);
	}
}
