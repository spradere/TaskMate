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
 * @file tm_softwareTimeCounter.c
 * @brief Implements STC.
 *
 */

#include "sysCore/tm_softwareTimeCounter.h"

#include "hal/public/hal_timerSTC.h"
#include "sysCore/modules.h"
#include "tm_libc/tm_syslog.h"
#include "hal/auto_hal_user.h"

static void tm_softwareTimeCounter(void);
void tm_softwareTimeCounterInit(void) { hal_timerSTCSetCallback(tm_softwareTimeCounter); }

void tm_softwareTimeCounter(void)
{
	// RTC decrement
	for( uint8_t i = 0; i < MOD_THREAD_COUNT; i++ )
	{
		mod_thread_item_t *mod = mod_threadGetPointer(i);
		if( mod->software_time_counter > 0 ) { mod->software_time_counter--; }
		//hal_usartWriteChar((uint8_t)(mod->software_time_counter));
		//hal_usartWriteChar(' ');

		/*if(mod->software_time_counter !=0)
		{
			tm_syslog(TM_STR("[tm_STC] mod[0x%04x]->0x%04x counter[%i] = %i\n"),
				mod,
				&mod->software_time_counter,
				i,
				mod->software_time_counter);
		}*/
	}
	//hal_usartWriteChar('\n');
	//hal_usartSendTXBuffer();

}
