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

#include "hal/auto_hal_system.h"
#include "hal/auto_hal_user.h"
//#include "sysCall/gpio.h"
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

void tm_schedulerStart(void){}

void tm_scheduler(void)
{
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
}
