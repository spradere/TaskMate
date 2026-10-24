/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sys_scheduler.c
 * @brief tm scheduler implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "sys_scheduler.h"

#include <stddef.h>
#include <stdint.h>

#include "interfaces/drv_timerContext.h"
#include "interfaces/tm_driver_have.h"
#ifdef TM_DRIVER_HAVE_TIMER_MONOTONIC
	#include "interfaces/drv_timerMonotonic.h"
#endif
#include "interfaces/hal_context.h"
#include "interfaces/hal_halt.h"
#include "interfaces/tm_macros.h"
#include "interfaces/tm_mod.h"
#include "interfaces/tm_runLevel.h"
#include "interfaces/tm_threads.h"
#include "system/sysCore/sys_threads.h"
#include "system/sysCore/sys_threads_data.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define SYSSCHEDULER_COUNT_CALLSPERSTC 10U

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static volatile tm_run_level_t scheduler_run_level = RL_RUN_CORE;
static uint8_t scheduler_stc_tick_count;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static hal_timerContextCallback_func_t tm_schedulerRR;
//static mod_thread_item_t *tm_schedulerSelectNext(mod_count_t current);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Scheduler life cycle
 * ---------------------------------------------*/

void tm_schedulerInit(void)
{
	scheduler_run_level = RL_RUN_CORE;
	scheduler_stc_tick_count = 0U;
	hal_timerContextControl(DRV_CTRL_INIT, 0U);
	hal_timerContextSetCallback(tm_schedulerRR);
}

void tm_schedulerStart(void)
{
	hal_timerContextControl(DRV_CTRL_START, 0U);

	mod_thread_item_t *mod = mod_threadGetPointer(0U);
	if( mod == NULL ) { hal_halt(); }

	hal_contextStart(&mod->context);
}

/* -----------------------------------------------
 * Run level
 * ---------------------------------------------*/

bool tm_schedulerRunLevelSet(tm_run_level_t run_level)
{
	if( (run_level < RL_RUN_CORE) || (run_level >= RL_LEVEL_COUNT) ||
		(run_level < scheduler_run_level) )
	{
		return false;
	}

	scheduler_run_level = run_level;
	return true;
}

tm_run_level_t tm_schedulerRunLevelGet(void) { return scheduler_run_level; }

/* -----------------------------------------------
 * Cooperative trigger
 * ---------------------------------------------*/

void tm_schedulerCoop(void) { hal_timerContextLoad(); }

/* -----------------------------------------------
 * Round-robin policy
 * ---------------------------------------------*/

/*
 * Called by the scheduling timer with a saved context. Check both stack canaries before
 * returning the next runnable thread's context to the HAL.
 */
static hal_context_t *tm_schedulerRR(hal_context_t *context)
{
	mod_thread_item_t *thread;
	mod_count_t current = mod_threadGetCurrent();
	
	// Save the current thread context
	thread = mod_threadGetPointer(current);
	if( thread == NULL ) { hal_halt(); }
	thread->context = *context;

	// Canary check
	if( thread->stack[MOD_STACK_LOW_CANARY_INDEX] != MOD_CANARY ) { hal_halt(); }
	if( thread->stack[thread->stack_size - MOD_STACK_FIRST_USABLE_INDEX] != MOD_CANARY )
	{
		hal_halt();
	}

	// Advance software time after ten scheduler interrupts.
	if( ++scheduler_stc_tick_count == SYSSCHEDULER_COUNT_CALLSPERSTC )
	{
		scheduler_stc_tick_count = 0U;
		mod_threadTickSTC();
	}

	// Switch thread
	tm_run_level_t thread_run_level;
	
	do
	{
		if( ++current == MOD_THREAD_COUNT ) { current = 0U; }
		thread = mod_threadGetPointer(current);
		thread_run_level = thread->run_level;
	} 
	while((thread_run_level == RL_RUN_NONE) || (thread_run_level > scheduler_run_level));
	
	mod_threadSetCurrent(current);
	thread = mod_threadGetPointer(current);

	// Canary check
	if( thread->stack[MOD_STACK_LOW_CANARY_INDEX] != MOD_CANARY ) { hal_halt(); }
	if( thread->stack[thread->stack_size - MOD_STACK_FIRST_USABLE_INDEX] != MOD_CANARY )
	{
		hal_halt();
	}

#ifdef TM_DRIVER_HAVE_TIMER_MONOTONIC
	thread->last_start_ticks_100us = hal_timerMonotonicRead();
#endif
	TM_CLEARBIT(thread->status, THREAD_BIT_YIELDED);
	return &thread->context;
}

/*static mod_thread_item_t *tm_schedulerSelectNext(mod_count_t current)
{
	tm_run_level_t active_run_level = scheduler_run_level;

	for( mod_count_t count = 0U; count < MOD_THREAD_COUNT; count++ )
	{
		if( ++current == MOD_THREAD_COUNT ) { current = 0U; }

		mod_thread_item_t *thread = mod_threadGetPointer(current);
		if( thread == NULL ) { hal_halt(); }
		tm_run_level_t thread_run_level = thread->run_level;
		if( (thread_run_level != RL_RUN_NONE) && (thread_run_level <= active_run_level) )
		{
			if( !mod_threadSetCurrent(current) ) { hal_halt(); }
			return thread;
		}
	}
	return 0;
}*/
