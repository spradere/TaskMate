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
 * @file scheduler.c
 * @brief scheduler implementation
 *
 * @todo add free channel
 */

#include "hal/hal_api.h"

#include "sysCore/TaskMate_private_extern.h"
#include "sysCore/scheduler.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// scheduler is called by timer interupt sub routine
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void scheduler()
{
	// enable global INT to let run timer3 RTC and usart1 sCLI
	hal_setGlobalInterupt();

	// stop timer1 prevent preemption of the scheduler itself -> panic
	// prevent scheduler eat thread time slice
	//**timer1Stop();
	hal_timerSchedulerStop();

	// todo -> add stack overflow test

	// todo -> add system wide error handler

	// switch context
	if( ++modules.thread_current == THREADS_COUNT ) { modules.thread_current = 0; }

	// I'm alive blink in board led
	static uint8_t alive_cnt = 0;
	if( ++alive_cnt > 250 )
	{
		hal_inBoardLed(HAL_IN_BOARD_LED_TOGGLE);
		alive_cnt = 0;
	}

	// cooperative handling
	sysCallClearFlag(FLAG_COOP);

	//**timer1Start();
	hal_timerSchedulerStart();
}
