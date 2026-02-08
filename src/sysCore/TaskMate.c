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
 * @file TaskMate.c
 * @brief Implements system init
 *
 */

#include <stdint.h>

#include "hal/auto_hal_define.h"
#include "hal/auto_hal_init.h"
#include "hal/auto_hal_system.h"
#include "hal/auto_hal_user.h"
#include "sysCall/sysCall.h"
#include "sysCore/modules.h"
#include "sysCore/runLevel.h"
#include "tm_libc/tm_stdio.h"
#include "tm_libc/tm_syslog.h"
#include "tm_version.h"

int main(void)
{
	// system startup
	hal_usartInit();
	hal_usartStart();

	tm_syslog("\n\n[boot] TaskMate %s boot\n", TASKMATE_VERSION);

	const sc_target_info_t *target;
	sc_targetGetInfo(&target);
	tm_syslog("[info] target : %s-%s-%s\n", target->arch, target->mcu, target->board);

	// system static allocation init
	tm_syslog("[boot] system static allocation\n");

	mod_driversAlloc();
	mod_threadsAlloc();
	rl_Alloc();

	// hal hardware init
	tm_syslog("[boot] hal hardware init\n");

	hal_archInit();
	hal_mcuInit();
	hal_boardInit();

	// start driver
	// TODO remove this code when run level is implemented

	for( uint8_t i = 0; i < MOD_DRIVER_COUNT; i++ )
	{
		mod_driver_item_t *mod = mod_driverGetPointer(i);
		(*(mod->init))();
		(*(mod->start))();
	}

	// RTC time test

	hal_rtc_time_t t;
	t.hours = 21;
	t.minutes = 41;
	t.seconds = 30;
	t.day = 7;
	t.month = 2;
	t.year = 26;

	// hal_rtcWrite(&t);
	hal_rtcRead(&t);

	tm_syslog("[info] date / time %02i/%02i/20%02i %02i:%02i\n", t.day, t.month, t.year, t.hours, t.minutes);

	char msg[64];
	tm_snprintf(msg, sizeof(msg), "TaskMate %s", TASKMATE_VERSION);
	hal_lcdClear();
	hal_lcdSetCursor(0, 0);
	hal_lcdWriteString(msg);

	tm_snprintf(msg, sizeof(msg), "%02i/%02i/20%02i %02i:%02i", t.day, t.month, t.year, t.hours, t.minutes);
	hal_lcdSetCursor(1, 0);
	hal_lcdWriteString(msg);

	// jump to current thread for first call and start system by enabling interrupts
	tm_syslog("[boot] start round-robin scheduler\n");

	mod_threadSetCurrent(0);
	mod_thread_item_t *mod = mod_threadGetPointer(mod_threadGetCurrent());
	hal_setStackPointer((uintptr_t)mod->stack_pointer);

	hal_contextRestore();
	hal_setGlobalInterupt();
	hal_returnFromInterupt();

	return 0; // You should never get here
}
