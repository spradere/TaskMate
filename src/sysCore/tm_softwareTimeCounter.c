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
 * @file tm_softwareTimeCounter.c
 * @brief Implements STC.
 *
 */

#include "sysCore/tm_softwareTimeCounter.h"

#include "sysCore/modules.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// softwareTimeCounter is called by hal_timerSTC interrupt sub routine
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void tm_softwareTimeCounter(void)
{
	// RTC decrement

	for( uint8_t i = 0; i < MOD_THREAD_COUNT; i++ )
	{
		mod_thread_item_t *mod = mod_threadGetPointer(i);
		if( mod->software_time_counter > 0 ) { mod->software_time_counter--; }
	}
}
