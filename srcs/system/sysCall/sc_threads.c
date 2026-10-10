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

#include <stddef.h>

#include "interfaces/hal_atomic.h"
#include "interfaces/tm_mod.h"
#include "interfaces/tm_runLevel.h"
#include "system/sysCall/sc_string.h"
#include "system/sysCore/sys_scheduler.h"
#include "system/sysCore/sys_threads.h"

/* -----------------------------------------------
 * Thread name catalog
 * ---------------------------------------------*/

// [autoCode_tag] thread_name_catalog
#include "thread_name_catalog.inc"
// [/tag]

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static bool sc_threadGetId(const char *name, mod_count_t *id);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Run level
 * ---------------------------------------------*/

tm_run_level_t sc_runLevelGet(void)
{
	hal_atomic_state_t state = hal_atomicStart();
	tm_run_level_t run_level = tm_schedulerRunLevelGet();
	hal_atomicEnd(state);
	return run_level;
}

bool sc_runLevelSet(tm_run_level_t run_level)
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

mod_count_t sc_threadGetCount(void) { return MOD_THREAD_COUNT; }

bool sc_threadGetInfo(mod_count_t id, const tm_string_t **name, tm_run_level_t *run_level,
					  uint16_t *stack_size_bytes)
{
	if( (id >= MOD_THREAD_COUNT) || (name == NULL) || (run_level == NULL) ||
		(stack_size_bytes == NULL) )
	{
		return false;
	}

	hal_atomic_state_t state = hal_atomicStart();
	*name = thread_name_catalog[id];
	*run_level = mod_threadRunLevelGet(id);
	*stack_size_bytes = mod_threadStackSizeGet(id);
	hal_atomicEnd(state);

	return *name != NULL;
}

bool sc_threadGetStackDepth(mod_count_t id, uint16_t *depth_bytes)
{
	if( (id >= MOD_THREAD_COUNT) || (depth_bytes == NULL) ) { return false; }

	hal_atomic_state_t state = hal_atomicStart();
	*depth_bytes = mod_threadStackDepthGet(id);
	hal_atomicEnd(state);
	return true;
}

/* -----------------------------------------------
 * Thread life cycle
 * ---------------------------------------------*/

void sc_threadSetInitialized(void)
{
	hal_atomic_state_t state = hal_atomicStart();
	mod_threadSetInitialized(mod_threadGetCurrent());
	hal_atomicEnd(state);
}

bool sc_threadStart(const char *name, tm_run_level_t initial_run_level)
{
	mod_count_t id;
	if( initial_run_level >= RL_LEVEL_COUNT ) { return false; }
	if( !sc_threadGetId(name, &id) ) { return false; }

	hal_atomic_state_t state = hal_atomicStart();
	mod_threadStart(id, initial_run_level);
	hal_atomicEnd(state);
	return true;
}

bool sc_threadStop(const char *name)
{
	mod_count_t id;
	if( !sc_threadGetId(name, &id) ) { return false; }

	hal_atomic_state_t state = hal_atomicStart();
	mod_threadStop(id);
	hal_atomicEnd(state);
	return true;
}

bool sc_threadRunLevelIsReady(tm_run_level_t run_level)
{
	if( (run_level == RL_RUN_NONE) || (run_level >= RL_LEVEL_COUNT) ) { return false; }

	hal_atomic_state_t state = hal_atomicStart();
	bool ready = mod_threadsRunLevelIsReady(run_level);
	hal_atomicEnd(state);

	return ready;
}

/* -----------------------------------------------
 * Cooperative scheduling
 * ---------------------------------------------*/

/*
 * Request an early timer switch, then wait until the scheduler clears this thread's
 * yielded bit when it is selected again.
 */
void sc_coopYield(void)
{
	hal_atomic_state_t state = hal_atomicStart();
	mod_count_t id = mod_threadGetCurrent();
	mod_threadSetYielded(id);
	tm_schedulerCoop();
	hal_atomicEnd(state);
	while( mod_threadIsYielded(id) );
}

/* -----------------------------------------------
 * Private helpers
 * ---------------------------------------------*/

static bool sc_threadGetId(const char *name, mod_count_t *id)
{
	if( (name == NULL) || (id == NULL) ) { return false; }

	for( mod_count_t i = 0U; i < MOD_THREAD_COUNT; i++ )
	{
		const tm_string_t *thread_name = thread_name_catalog[i];
		if( (thread_name != NULL) &&
			sc_stringCompare(*thread_name, TM_STR_RAM(name), MOD_NAME_SIZE_MAX) == 0U )
		{
			*id = i;
			return true;
		}
	}

	return false;
}
