/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
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
#include "sysCall/gpio.h"
#include "sysCall/sysCall.h"
#include "sysCore/modules.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// scheduler is called by hal_timerScheduler interupt sub routine
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void tm_scheduler(void)
{
	// enable global INT to let run hal_timerRTC and hal_usart sCLI
	hal_setGlobalInterupt();

	// stop hal_timerScheduler prevent preemption of the scheduler itself -> panic
	// prevent scheduler eat thread time slice
	hal_timerSchedulerStop();

	// switch thread
	uint8_t current = mod_threadGetCurrent();
	if( ++current == MODULES_THREAD_COUNT ) { mod_threadSetCurrent(0); }
	else { mod_threadSetCurrent(current); }

	// I'm alive blink in board led
	static uint8_t alive_cnt = 0;
	if( ++alive_cnt > 250 )
	{
		gpio_signalToggle(GPIO_SIGNAL_INBOARD_LED);
		alive_cnt = 0;
	}

	// cooperative handling
	sc_flagClear(FLAG_COOP);

	hal_timerSchedulerStart();
}
