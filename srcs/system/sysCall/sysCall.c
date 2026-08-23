/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sysCall.c
 * @brief sys call implementation.
 *
 */

#include "sysCall.h"

#include "hal/public/atomic.h"
#include "hal/public/timerSched.h"
#include "interfaces/drivers.h"
#include "interfaces/macros.h"
#include "interfaces/modules_define.h"
#include "interfaces/runLevel_define.h"
#include "system/sysCore/modules.h"
#include "system/sysCore/tm_scheduler.h"
#include "tm_libc/tm_string.h"

static mod_thread_item_t *sc_threadGetPointer(const char *name);
static mod_driver_item_t *sc_driverGetPointer(const char *name);
static bool sc_driverControl(const char *name, uint8_t command);

void sc_threadSetSTC(uint16_t count)
{
	hal_atomic_state_t state = hal_atomicStart();
	mod_threadSetSTC(count);
	hal_atomicEnd(state);
}

uint16_t sc_threadGetSTC(void)
{
	hal_atomic_state_t state = hal_atomicStart();
	uint16_t timer = mod_threadGetSTC();
	hal_atomicEnd(state);
	return timer;
}

uint16_t sc_threadGetCount(void) { return TM_MOD_THREAD_COUNT; }

bool sc_threadGetInfo(uint16_t id, const tm_string_t **name, uint8_t *run_level)
{
	if( (id >= TM_MOD_THREAD_COUNT) || (name == 0) || (run_level == 0) ) { return false; }

	hal_atomic_state_t state = hal_atomicStart();
	mod_thread_item_t *thread = mod_threadGetPointer((uint8_t)id);
	*name = thread->name;
	*run_level = RL_GET_RUN_LEVEL(thread->status);
	hal_atomicEnd(state);

	return *name != 0;
}

bool sc_threadStart(const char *name, uint8_t initial_run_level)
{
	mod_thread_item_t *thread = sc_threadGetPointer(name);
	if( thread == 0 ) { return false; }

	hal_atomic_state_t state = hal_atomicStart();

	if( thread->saved_run_level == RL_RUN_NONE ) { thread->saved_run_level = initial_run_level; }
	else
	{
		thread->status &= (uint8_t)~RL_LEVEL_MASK;
		thread->status |= thread->saved_run_level;
	}

	hal_atomicEnd(state);
	return true;
}

bool sc_threadStop(const char *name)
{
	mod_thread_item_t *thread = sc_threadGetPointer(name);
	if( thread == 0 ) { return false; }

	hal_atomic_state_t state = hal_atomicStart();
	uint8_t current_run_level = RL_GET_RUN_LEVEL(thread->status);

	thread->saved_run_level = current_run_level;
	thread->status &= (uint8_t)~RL_LEVEL_MASK;

	hal_atomicEnd(state);
	return true;
}

uint16_t sc_driverGetCount(void) { return TM_MOD_DRIVER_COUNT; }

bool sc_driverGetInfo(uint16_t id, const tm_string_t **name, uint8_t *run_level,
					  uint8_t *status_bits)
{
	if( (id >= TM_MOD_DRIVER_COUNT) || (name == 0) || (run_level == 0) || (status_bits == 0) )
	{
		return false;
	}

	mod_driver_item_t *driver = mod_driverGetPointer((uint8_t)id);
	if( (driver->name == 0) || (driver->control == 0) ) { return false; }

	*name = driver->name;
	*run_level = driver->control(DRV_CTRL_RLGET, 0);
	*status_bits = 0;

	for( uint8_t bit = DRV_BIT_INIT; bit <= DRV_BIT_DEAD; bit++ )
	{
		if( driver->control(DRV_CTRL_GETBIT, bit) != 0 ) { TM_SETBIT(*status_bits, bit); }
	}

	return *name != 0;
}

bool sc_driverInit(const char *name) { return sc_driverControl(name, DRV_CTRL_INIT); }

bool sc_driverStart(const char *name) { return sc_driverControl(name, DRV_CTRL_START); }

bool sc_driverStop(const char *name) { return sc_driverControl(name, DRV_CTRL_STOP); }

void sc_coopYield(void)
{
	hal_atomic_state_t state = hal_atomicStart();
	mod_thread_item_t *thread = mod_threadGetPointer(mod_threadGetCurrent());
	TM_SETBIT(thread->status, TM_MOD_THREAD_YIELDED);
	tm_schedulerCoop();
	hal_atomicEnd(state);
	while( TM_GETBIT(thread->status, TM_MOD_THREAD_YIELDED) );
}

static mod_thread_item_t *sc_threadGetPointer(const char *name)
{
	if( name == 0 ) { return 0; }

	for( uint8_t i = 0; i < TM_MOD_THREAD_COUNT; i++ )
	{
		mod_thread_item_t *thread = mod_threadGetPointer(i);
		if( (thread->name != 0) &&
			tm_strncmp(*thread->name, TM_STR_RAM(name), TM_MOD_NAME_SIZE_MAX) == 0 )
		{
			return thread;
		}
	}

	return 0;
}

static mod_driver_item_t *sc_driverGetPointer(const char *name)
{
	if( name == 0 ) { return 0; }

	for( uint8_t i = 0; i < TM_MOD_DRIVER_COUNT; i++ )
	{
		mod_driver_item_t *driver = mod_driverGetPointer(i);
		if( (driver->name != 0) && (driver->control != 0) &&
			tm_strncmp(*driver->name, TM_STR_RAM(name), TM_MOD_NAME_SIZE_MAX) == 0 )
		{
			return driver;
		}
	}

	return 0;
}

static bool sc_driverControl(const char *name, uint8_t command)
{
	mod_driver_item_t *driver = sc_driverGetPointer(name);
	if( driver == 0 ) { return false; }

	return driver->control(command, 0) == 0;
}
