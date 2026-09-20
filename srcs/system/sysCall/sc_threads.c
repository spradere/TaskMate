/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sc_threads.c
 * @brief Thread and system syscall implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "sc_threads.h"

#include "interfaces/hal_atomic.h"
#include "interfaces/tm_macros.h"
#include "interfaces/tm_modules.h"
#include "interfaces/tm_runLevel.h"
#include "interfaces/tm_threads.h"
#include "system/sysCall/sc_string.h"
#include "system/sysCore/sys_modules.h"
#include "system/sysCore/sys_scheduler.h"

/* -----------------------------------------------
 * Thread name catalog
 * ---------------------------------------------*/

// [autoCode_tag] thread_name_catalog
#include "thread_name_catalog.inc"
// [/tag]

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static mod_thread_item_t *sc_threadGetPointer(const char *name);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Run level
 * ---------------------------------------------*/

uint8_t sc_runLevelGet(void)
{
	hal_atomic_state_t state = hal_atomicStart();
	uint8_t run_level = tm_schedulerRunLevelGet();
	hal_atomicEnd(state);
	return run_level;
}

bool sc_runLevelSet(uint8_t run_level)
{
	hal_atomic_state_t state = hal_atomicStart();
	bool result = tm_schedulerRunLevelSet(run_level);
	hal_atomicEnd(state);
	return result;
}

/* -----------------------------------------------
 * Software time counters
 * ---------------------------------------------*/

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

/* -----------------------------------------------
 * Thread metadata
 * ---------------------------------------------*/

uint16_t sc_threadGetCount(void) { return MOD_THREAD_COUNT; }

bool sc_threadGetInfo(uint16_t id, const tm_string_t **name, uint8_t *run_level)
{
	if( (id >= MOD_THREAD_COUNT) || (name == 0) || (run_level == 0) ) { return false; }

	hal_atomic_state_t state = hal_atomicStart();
	mod_thread_item_t *thread = mod_threadGetPointer((uint8_t)id);
	*name = thread_name_catalog[id];
	*run_level = RL_GET_RUN_LEVEL(thread->status);
	hal_atomicEnd(state);

	return *name != 0;
}

/* -----------------------------------------------
 * Thread life cycle
 * ---------------------------------------------*/

void sc_threadSetInitialized(void)
{
	hal_atomic_state_t state = hal_atomicStart();
	mod_thread_item_t *thread = mod_threadGetPointer(mod_threadGetCurrent());
	TM_SETBIT(thread->status, THREAD_BIT_INITIALIZED);
	hal_atomicEnd(state);
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

bool sc_threadRunLevelIsReady(uint8_t run_level)
{
	if( (run_level == RL_RUN_NONE) || (run_level >= RL_LEVEL_COUNT) ) { return false; }

	bool ready = true;
	hal_atomic_state_t state = hal_atomicStart();
	for( uint8_t i = 0; i < MOD_THREAD_COUNT; i++ )
	{
		mod_thread_item_t *thread = mod_threadGetPointer(i);
		if( (RL_GET_RUN_LEVEL(thread->status) == run_level) &&
			((TM_GETBIT(thread->status, THREAD_BIT_INITIALIZED) == 0) ||
			 (TM_GETBIT(thread->status, THREAD_BIT_DEAD) != 0)) )
		{
			ready = false;
			break;
		}
	}
	hal_atomicEnd(state);

	return ready;
}

/* -----------------------------------------------
 * Cooperative scheduling
 * ---------------------------------------------*/

void sc_coopYield(void)
{
	hal_atomic_state_t state = hal_atomicStart();
	mod_thread_item_t *thread = mod_threadGetPointer(mod_threadGetCurrent());
	TM_SETBIT(thread->status, THREAD_BIT_YIELDED);
	tm_schedulerCoop();
	hal_atomicEnd(state);
	while( TM_GETBIT(thread->status, THREAD_BIT_YIELDED) );
}

/* -----------------------------------------------
 * Private helpers
 * ---------------------------------------------*/

static mod_thread_item_t *sc_threadGetPointer(const char *name)
{
	if( name == 0 ) { return 0; }

	for( uint8_t i = 0; i < MOD_THREAD_COUNT; i++ )
	{
		mod_thread_item_t *thread = mod_threadGetPointer(i);
		const tm_string_t *thread_name = thread_name_catalog[i];
		if( (thread_name != 0) &&
			sc_stringCompare(*thread_name, TM_STR_RAM(name), MOD_NAME_SIZE_MAX) == 0 )
		{
			return thread;
		}
	}

	return 0;
}
