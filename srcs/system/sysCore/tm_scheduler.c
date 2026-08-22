/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_scheduler.c
 * @brief tm scheduler implementation.
 *
 */

#include "tm_scheduler.h"

#include <stdint.h>

#include "hal/public/context.h"
#include "hal/public/interrupt.h"
#include "hal/public/panic.h"
#include "hal/public/stack.h"
#include "hal/public/timerSched.h"
#include "interfaces/drivers.h"
#include "interfaces/macros.h"
#include "system/sysCore/modules.h"
#include "system/sysCore/modules_list.h"
#include "tm_libc/tm_string.h"

static hal_timerSchedCallback_func_t tm_schedulerRR;

void tm_schedulerInit(void)
{
	hal_timerSchedControl(DRV_CTRL_INIT, 0);
	hal_timerSchedSetCallback(tm_schedulerRR);
}

void tm_schedulerStart(void)
{
	hal_timerSchedControl(DRV_CTRL_START, 0);

	mod_threadSetCurrent(0);
	mod_thread_item_t *mod = mod_threadGetPointer(mod_threadGetCurrent());
	hal_setStackPointer(mod->stack_pointer);

	hal_contextRestore();
	hal_setGlobalInterupt();
	hal_returnFromInterupt();
}

void tm_schedulerCoop(void) { hal_timerSchedLoad(); }

hal_stack_word_t *tm_schedulerRR(hal_stack_word_t *stack_pointer)
{
	mod_thread_item_t *thread;

	// save current thread context
	thread = mod_threadGetPointer(mod_threadGetCurrent());
	thread->stack_pointer = stack_pointer;

	// canary check
	if( thread->canary_low != TM_MOD_CANARY ) { panic(TM_STR("canary low 1")); }
	if( thread->canary_high != TM_MOD_CANARY ) { panic(TM_STR("canary high 1")); }

	// switch thread
	uint8_t current = mod_threadGetCurrent();

	if( ++current == TM_MOD_THREAD_COUNT ) { current = 0; }
	mod_threadSetCurrent(current);
	thread = mod_threadGetPointer(current);

	// canary check
	if( thread->canary_low != TM_MOD_CANARY ) { panic(TM_STR("canary low 2")); }
	if( thread->canary_high != TM_MOD_CANARY ) { panic(TM_STR("canary high 2")); }

	TM_CLEARBIT(thread->status, TM_MOD_THREAD_YIELDED);
	return thread->stack_pointer;
}
