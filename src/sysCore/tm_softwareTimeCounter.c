/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD 2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_softwareTimeCounter.c
 * @brief sysCore tm_softwareTimeCounter implementation.
 *
 */

#include "sysCore/tm_softwareTimeCounter.h"

#include "hal/public/hal_timerSTC.h"
#include "sysCore/modules.h"

static void tm_softwareTimeCounter(void);
void tm_softwareTimeCounterInit(void) { hal_timerSTCSetCallback(tm_softwareTimeCounter); }

void tm_softwareTimeCounter(void)
{
	// RTC decrement
	for( uint8_t i = 0; i < MOD_THREAD_COUNT; i++ )
	{
		mod_thread_item_t *mod = mod_threadGetPointer(i);
		if( mod->software_time_counter > 0 ) { mod->software_time_counter--; }
	}
}
