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
#include "hal/auto_hal_user.h"
#include "sysCall/sysCall.h"
#include "sysCore/modules.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// scheduler is called by hal_timerScheduler interrupt subroutine
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

static void tm_scheduler(void);

void tm_schedulerInit(void)
{
	hal_timer1SetCallback(tm_scheduler);
}

void tm_schedulerStart(void)
{
	mod_threadSetCurrent(0);
	mod_thread_item_t *mod = mod_threadGetPointer(mod_threadGetCurrent());
	hal_setStackPointer((uintptr_t)mod->stack_pointer);

	hal_contextRestore();
	hal_setGlobalInterupt();
	hal_returnFromInterupt();
}

void tm_scheduler(void)
{
	mod_thread_item_t *mod;

	// save current thread context
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		hal_contextSave();
		mod = mod_threadGetPointer(mod_threadGetCurrent());
		mod->stack_pointer = (hal_stack_word_t *)hal_getStackPointer();
	}

	// enable global INT to let run hal_timerRTC and hal_usart sCLI
	hal_setGlobalInterupt();

	// stop hal_timerScheduler prevent preemption of the scheduler itself -> panic
	// prevent scheduler eat thread time slice
	hal_timer1Stop();

	// switch thread
	uint8_t current = mod_threadGetCurrent();
	if( ++current == MOD_THREAD_COUNT ) { mod_threadSetCurrent(0); }
	else { mod_threadSetCurrent(current); }

	// cooperative handling
	sc_flagClear(FLAG_COOP);

	hal_timer1Start();

	// restore next thread context
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		mod = mod_threadGetPointer(mod_threadGetCurrent());
		hal_setStackPointer((uintptr_t)mod->stack_pointer);
		hal_contextRestore();
		hal_returnFromInterupt();
	}
}
