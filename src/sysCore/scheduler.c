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

#include "hal/hal_user_api.h"
#include "hal/hal_system_critical_api.h"

#include "sysCore/autoAlloc.h" // get THREADS_COUNT
#include "sysCore/modules.h"
#include "sysCore/scheduler.h"
#include "sysCall/sysCall.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// scheduler is called by hal_timerScheduler interupt sub routine
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void scheduler()
{
	// enable global INT to let run hal_timerRTC and hal_usart sCLI
	hal_setGlobalInterupt();

	// stop hal_timerScheduler prevent preemption of the scheduler itself -> panic
	// prevent scheduler eat thread time slice
	hal_timerSchedulerStop();

	// todo -> add stack overflow test

	// todo -> add system wide error handler

	// switch thread
	uint8_t current = moduleThreadGetCurrent();
	if( ++current == THREADS_COUNT ) { moduleThreadSetCurrent(0); }
	else { moduleThreadSetCurrent(current); }

	// I'm alive blink in board led
	static uint8_t alive_cnt = 0;
	if( ++alive_cnt > 250 )
	{
		hal_inBoardLed(HAL_IN_BOARD_LED_TOGGLE);
		alive_cnt = 0;
	}

	// cooperative handling
	sysCallClearFlag(FLAG_COOP);

	hal_timerSchedulerStart();
}
