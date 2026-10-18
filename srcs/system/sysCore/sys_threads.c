/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sys_threads.c
 * @brief Static thread database implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "sys_threads.h"

#include <stddef.h>

#include "interfaces/hal_context.h"
#include "interfaces/hal_halt.h"
#include "interfaces/tm_macros.h"
#include "interfaces/tm_mod.h"
#include "interfaces/tm_runLevel.h"
#include "interfaces/tm_threads.h"
#include "system/sysCore/sys_threads_data.h"
#include "system/sysCore/sys_threads_list.h"

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static mod_thread_item_t threads[MOD_THREAD_COUNT];
static mod_count_t thread_current;

// [autoCode_tag] thread_stacks
#include "thread_stacks.inc"
// [/tag]

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void mod_threadStackInit(mod_thread_item_t *thread);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

bool mod_threadSetCurrent(mod_count_t id)
{
	if( id >= MOD_THREAD_COUNT ) { return false; }
	thread_current = id;
	return true;
}
mod_count_t mod_threadGetCurrent(void) { return thread_current; }

bool mod_threadSetSTC(uint16_t count)
{
	mod_thread_item_t *thread = mod_threadGetPointer(thread_current);
	if( thread == NULL ) { return false; }
	thread->software_time_counter = count;
	return true;
}
bool mod_threadGetSTC(uint16_t *count)
{
	mod_thread_item_t *thread = mod_threadGetPointer(thread_current);
	if( (thread == NULL) || (count == NULL) ) { return false; }
	*count = thread->software_time_counter;
	return true;
}

void mod_threadTickSTC(void)
{
	for( mod_count_t id = 0U; id < MOD_THREAD_COUNT; id++ )
	{
		if( threads[id].software_time_counter > 0U ) { threads[id].software_time_counter--; }
	}
}

bool mod_threadRunLevelGet(mod_count_t id, tm_run_level_t *run_level)
{
	mod_thread_item_t *thread = mod_threadGetPointer(id);
	if( (thread == NULL) || (run_level == NULL) ) { return false; }
	*run_level = thread->run_level;
	return true;
}

bool mod_threadStackSizeGet(mod_count_t id, uint16_t *size_bytes)
{
	mod_thread_item_t *thread = mod_threadGetPointer(id);
	if( (thread == NULL) || (size_bytes == NULL) ) { return false; }
	*size_bytes = (uint16_t)(thread->stack_size * sizeof(hal_stack_word_t));
	return true;
}

bool mod_threadStackDepthGet(mod_count_t id, uint16_t *depth_bytes)
{
	mod_thread_item_t *thread = mod_threadGetPointer(id);
	if( (thread == NULL) || (depth_bytes == NULL) ) { return false; }
	const volatile hal_stack_word_t *stack = thread->stack;
	const uint16_t usable_words = thread->stack_size - MOD_STACK_CANARY_WORD_COUNT;
	uint16_t unused_words = 0U;

	while( (unused_words < usable_words) &&
		   (stack[unused_words + MOD_STACK_FIRST_USABLE_INDEX] == MOD_STACK_PATTERN) )
	{
		unused_words++;
	}

	*depth_bytes = (uint16_t)((usable_words - unused_words) * sizeof(hal_stack_word_t));
	return true;
}

bool mod_threadSetInitialized(mod_count_t id)
{
	mod_thread_item_t *thread = mod_threadGetPointer(id);
	if( thread == NULL ) { return false; }
	TM_SETBIT(thread->status, THREAD_BIT_INITIALIZED);
	return true;
}

bool mod_threadStart(mod_count_t id, tm_run_level_t initial_run_level)
{
	mod_thread_item_t *thread = mod_threadGetPointer(id);
	if( thread == NULL ) { return false; }

	if( thread->saved_run_level == RL_RUN_NONE ) { thread->saved_run_level = initial_run_level; }
	else
	{
		thread->run_level = thread->saved_run_level;
	}
	return true;
}

bool mod_threadStop(mod_count_t id)
{
	mod_thread_item_t *thread = mod_threadGetPointer(id);
	if( thread == NULL ) { return false; }

	thread->saved_run_level = thread->run_level;
	thread->run_level = RL_RUN_NONE;
	return true;
}

bool mod_threadsRunLevelIsReady(tm_run_level_t run_level)
{
	for( mod_count_t id = 0U; id < MOD_THREAD_COUNT; id++ )
	{
		if( (threads[id].run_level == run_level) &&
			((TM_GETBIT(threads[id].status, THREAD_BIT_INITIALIZED) == 0U) ||
			 (TM_GETBIT(threads[id].status, THREAD_BIT_DEAD) != 0U)) )
		{
			return false;
		}
	}

	return true;
}

bool mod_threadSetYielded(mod_count_t id)
{
	mod_thread_item_t *thread = mod_threadGetPointer(id);
	if( thread == NULL ) { return false; }
	TM_SETBIT(thread->status, THREAD_BIT_YIELDED);
	return true;
}

bool mod_threadIsYielded(mod_count_t id, bool *yielded)
{
	mod_thread_item_t *thread = mod_threadGetPointer(id);
	if( (thread == NULL) || (yielded == NULL) ) { return false; }
	*yielded = TM_GETBIT(thread->status, THREAD_BIT_YIELDED) != 0U;
	return true;
}

mod_thread_item_t *mod_threadGetPointer(mod_count_t id)
{
	if( id >= MOD_THREAD_COUNT ) { return NULL; }
	return &threads[id];
}

static void mod_threadStackInit(mod_thread_item_t *thread)
{
	for( uint16_t stack_index = 0U; stack_index < thread->stack_size; stack_index++ )
	{
		thread->stack[stack_index] = MOD_STACK_PATTERN;
	}
	thread->stack[MOD_STACK_LOW_CANARY_INDEX] = MOD_CANARY;
	thread->stack[thread->stack_size - MOD_STACK_FIRST_USABLE_INDEX] = MOD_CANARY;
}

void mod_threadsAlloc(void)
{
	// [autoCode_tag] threads_alloc
#include "threads_alloc.inc"
	// [/tag]
}
