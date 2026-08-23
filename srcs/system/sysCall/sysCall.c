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
#include "interfaces/macros.h"
#include "interfaces/modules_define.h"
#include "interfaces/runLevel_define.h"
#include "system/sysCore/modules.h"
#include "system/sysCore/tm_scheduler.h"
#include "tm_libc/tm_string.h"

static bool sc_threadNameEquals(const tm_string_t *registered_name, const char *name);
static bool sc_threadRunLevelSet(const char *name, uint8_t run_level);

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

bool sc_threadStart(const char *name) { return sc_threadRunLevelSet(name, RL_RUN_USER); }

bool sc_threadStop(const char *name) { return sc_threadRunLevelSet(name, RL_RUN_NONE); }

void sc_coopYield(void)
{
	hal_atomic_state_t state = hal_atomicStart();
	mod_thread_item_t *thread = mod_threadGetPointer(mod_threadGetCurrent());
	TM_SETBIT(thread->status, TM_MOD_THREAD_YIELDED);
	tm_schedulerCoop();
	hal_atomicEnd(state);
	while( TM_GETBIT(thread->status, TM_MOD_THREAD_YIELDED) );
}

static bool sc_threadNameEquals(const tm_string_t *registered_name, const char *name)
{
	if( (registered_name == 0) || (name == 0) ) { return false; }

	for( uint8_t i = 0; i < TM_MOD_NAME_SIZE_MAX; i++ )
	{
		char registered_char = hal_string_getChar(registered_name, i);
		if( registered_char != name[i] ) { return false; }
		if( registered_char == 0 ) { return true; }
	}

	return false;
}

static bool sc_threadRunLevelSet(const char *name, uint8_t run_level)
{
	if( name == 0 ) { return false; }

	for( uint8_t i = 0; i < TM_MOD_THREAD_COUNT; i++ )
	{
		mod_thread_item_t *thread = mod_threadGetPointer(i);
		if( sc_threadNameEquals(thread->name, name) )
		{
			hal_atomic_state_t state = hal_atomicStart();
			thread->status &= (uint8_t)~RL_RUN_LEVEL_MASK;
			thread->status |= run_level;
			hal_atomicEnd(state);
			return true;
		}
	}

	return false;
}
