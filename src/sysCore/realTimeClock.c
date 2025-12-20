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
 * @file rtc.c
 * @brief Implements internal/external RTC.
 *
 * @todo Add I2C external clock
 */

#include "sysCore/autoAlloc.h" // get THREADS_COUNT
#include "sysCore/modules.h"
#include "sysCore/realTimeClock.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// realTimeClock is called by hal_timerRTC interupt sub routine
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void realTimeClock(void)
{
	// RTC decrement

	for( uint8_t i = 0; i < THREADS_COUNT; i++ )
	{
		module_item_thread_t *mod_t = moduleThreadGetPointer(i);
		if( mod_t->real_time_counter > 0 ) { mod_t->real_time_counter--; }
	}
}
