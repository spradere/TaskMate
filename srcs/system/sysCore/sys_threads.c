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

#include "interfaces/hal_context.h"
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

void mod_threadSetCurrent(mod_count_t id) { thread_current = id; }
mod_count_t mod_threadGetCurrent(void) { return thread_current; }

void mod_threadSetSTC(uint16_t count) { threads[thread_current].software_time_counter = count; }
uint16_t mod_threadGetSTC(void) { return threads[thread_current].software_time_counter; }

void mod_threadTickSTC(void)
{
	for( mod_count_t id = 0U; id < MOD_THREAD_COUNT; id++ )
	{
		if( threads[id].software_time_counter > 0U ) { threads[id].software_time_counter--; }
	}
}

tm_run_level_t mod_threadRunLevelGet(mod_count_t id)
{
	return RL_GET_RUN_LEVEL(threads[id].status);
}

uint16_t mod_threadStackSizeGet(mod_count_t id)
{
	return (uint16_t)(threads[id].stack_size * sizeof(hal_stack_word_t));
}

uint16_t mod_threadStackDepthGet(mod_count_t id)
{
	const volatile hal_stack_word_t *stack = threads[id].stack;
	const uint16_t usable_words = threads[id].stack_size - MOD_STACK_CANARY_WORD_COUNT;
	uint16_t unused_words = 0U;

	while( (unused_words < usable_words) &&
		   (stack[unused_words + MOD_STACK_FIRST_USABLE_INDEX] == MOD_STACK_PATTERN) )
	{
		unused_words++;
	}

	return (uint16_t)((usable_words - unused_words) * sizeof(hal_stack_word_t));
}

void mod_threadSetInitialized(mod_count_t id)
{
	TM_SETBIT(threads[id].status, THREAD_BIT_INITIALIZED);
}

void mod_threadStart(mod_count_t id, tm_run_level_t initial_run_level)
{
	mod_thread_item_t *thread = &threads[id];

	if( thread->saved_run_level == RL_RUN_NONE ) { thread->saved_run_level = initial_run_level; }
	else
	{
		thread->status &= (tm_thread_status_t)~RL_LEVEL_MASK;
		thread->status |= thread->saved_run_level;
	}
}

void mod_threadStop(mod_count_t id)
{
	mod_thread_item_t *thread = &threads[id];

	thread->saved_run_level = RL_GET_RUN_LEVEL(thread->status);
	thread->status &= (tm_thread_status_t)~RL_LEVEL_MASK;
}

bool mod_threadsRunLevelIsReady(tm_run_level_t run_level)
{
	for( mod_count_t id = 0U; id < MOD_THREAD_COUNT; id++ )
	{
		if( (RL_GET_RUN_LEVEL(threads[id].status) == run_level) &&
		((TM_GETBIT(threads[id].status, THREAD_BIT_INITIALIZED) == 0U) ||
		 (TM_GETBIT(threads[id].status, THREAD_BIT_DEAD) != 0U)) )
		{
			return false;
		}
	}

	return true;
}

void mod_threadSetYielded(mod_count_t id) { TM_SETBIT(threads[id].status, THREAD_BIT_YIELDED); }

bool mod_threadIsYielded(mod_count_t id)
{
	return TM_GETBIT(threads[id].status, THREAD_BIT_YIELDED) != 0U;
}

mod_thread_item_t *mod_threadGetPointer(mod_count_t id) { return &threads[id]; }

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
