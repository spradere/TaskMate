/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file tm_scheduler.c
 * @brief scheduler implementation
 *
 */

#include "sysCore/tm_scheduler.h"

#include <stdint.h>
#include <util/atomic.h>

#include "hal/auto_hal_system.h"
#include "hal/public/hal_context.h"
#include "hal/public/hal_stack.h"
#include "hal/public/hal_timerSched.h"
#include "sysCall/sysCall.h"
#include "sysCall/panic.h"
#include "sysCore/modules.h"
#include "tm_libc/tm_stdio.h"


static void tm_schedulerRR(void);
static hal_stack_word_t *system_SP;

void tm_schedulerInit(void) { hal_timerSchedSetCallback(tm_schedulerRR); }

void tm_schedulerStart(void)
{
	mod_threadSetCurrent(0);
	mod_thread_item_t *mod = mod_threadGetPointer(mod_threadGetCurrent());

	system_SP = (hal_stack_word_t*)hal_getStackPointer();

	hal_setStackPointer((uintptr_t)mod->stack_pointer);

	hal_contextRestore();
	hal_setGlobalInterupt();
	hal_returnFromInterupt();
}

void tm_schedulerCoop(void) { tm_schedulerRR(); }

void tm_schedulerRR(void)
{
	mod_thread_item_t *thread;
	// save current thread context
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		thread = mod_threadGetPointer(mod_threadGetCurrent());
		thread->stack_pointer = (hal_stack_word_t *)hal_getStackPointer();
	}

	// canary check
	if(thread->canary_low != MOD_CANARY){panic("canary low 1");}
	if(thread->canary_high != MOD_CANARY){panic("canary high 1");}

	// enable global INT to let run hal_timerRTC and hal_usart sCLI
	//hal_setGlobalInterupt();

	// switch thread
	uint8_t current = mod_threadGetCurrent();

	if( ++current == MOD_THREAD_COUNT ) { current = 0; }
	mod_threadSetCurrent(current);

	// canary check
	if(thread->canary_low != MOD_CANARY){panic("canary low 2");}
	if(thread->canary_high != MOD_CANARY){panic("canary high 2");}

	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		thread = mod_threadGetPointer(mod_threadGetCurrent());
		hal_setStackPointer((uintptr_t)thread->stack_pointer);
	}
}
