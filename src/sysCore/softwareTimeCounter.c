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
 * @file softwareTimeCounter.c
 * @brief Implements internal/external RTC.
 *
 */

#include "sysCore/softwareTimeCounter.h"

#include "sysCore/modules.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// softwareTimeCounter is called by hal_timerSTC interupt sub routine
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void softwareTimeCounter(void)
{
	// RTC decrement

	for( uint8_t i = 0; i < MODULES_THREAD_COUNT; i++ )
	{
		module_item_thread_t *mod_t = moduleThreadGetPointer(i);
		if( mod_t->software_time_counter > 0 ) { mod_t->software_time_counter--; }
	}
}
