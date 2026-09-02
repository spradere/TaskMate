/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file TaskMate.c
 * @brief TaskMate implementation.
 *
 */

#include <stdint.h>

#include "hal/public/lcd.h"
#include "hal/public/panic.h"
#include "hal/public/rtc.h"
#include "interfaces/macros.h"
#include "interfaces/options.h"
#include "interfaces/TaskMate_info.h"
#include "system/sysCore/boot.h"
#include "system/sysCore/modules.h"
#include "system/sysCore/tm_scheduler.h"
#include "system/sysCore/tm_softwareTimeCounter.h"
#include "tm_libc/tm_stdio.h"
#include "tm_libc/tm_string.h"
#include "tm_libc/tm_syslog.h"

TM_STORE_FILE_NAME(file_name);

// Display hardware target information
#if VERBOSE_LEVEL > 0
	#pragma message "TM_VERSION  = " TM_VERSION
	#pragma message "BUILD  = " TM_STRINGIFY(BUILD_CNT)
#endif

int main(void)
{
	// System startup
	boot();

	tm_syslog(
		TM_STR("[info] %s v%i.%i build : %i\n"), &file_name, TM_VER_MAJOR, TM_VER_MINOR, TM_BUILD);

	/* *************************************************************************************************
	 * Test and experimental zone before the scheduler starts
	 * ************************************************************************************************/

	// External RTC module test
	tm_syslog(TM_STR("[info] hal RTC init\n"));
	hal_rtc_time_t t;
	t.hours = 18;
	t.minutes = 54;
	t.seconds = 45;
	t.day = 17;
	t.month = 8;
	t.year = 26;

	// hal_rtcWrite(&t);
	hal_rtcRead(&t);

	tm_syslog(TM_STR("[info] date & time : %02i/%02i/20%02i %02i:%02i\n"),
			  t.day,
			  t.month,
			  t.year,
			  t.hours,
			  t.minutes);

	char msg[40];
	tm_snprintf(
		msg, sizeof(msg), TM_STR("TaskMate %i.%i %i"), TM_VER_MAJOR, TM_VER_MINOR, TM_BUILD);
	hal_lcdClear();
	hal_lcdSetCursor(0, 0);
	hal_lcdWriteString(TM_STR_RAM(msg));

	tm_snprintf(msg,
				sizeof(msg),
				TM_STR("%02i/%02i/20%02i %02i:%02i"),
				t.day,
				t.month,
				t.year,
				t.hours,
				t.minutes);
	hal_lcdSetCursor(1, 0);
	hal_lcdWriteString(TM_STR_RAM(msg));

	/*// test error catalog
	tm_syslog(TM_STR("[info] error catalog\n"));
	for( uint8_t i = 0; i < ERROR_COUNT; i++ )
	{
		const tm_string_t *err_msg = err_getMessage(i);
		tm_syslog(TM_STR("\t%i [0x%04x->0x%04x] <%s>\n"), i, &err_msg, err_msg, err_msg);
	}*/

	/*// display module names
	tm_syslog(TM_STR("[modules] drivers\n"));

	for( uint8_t num = 0; num < TM_MOD_DRIVER_COUNT; num++ )
	{
		mod_driver_item_t *mod_d;
		mod_d = mod_driverGetPointer(num);
		tm_syslog(TM_STR("\t%i %s\n"), num, mod_d->name);
	}

	tm_syslog(TM_STR("[modules] threads\n"));

	for( uint8_t num = 0; num < TM_MOD_THREAD_COUNT; num++ )
	{
		mod_thread_item_t *mod_t;
		mod_t = mod_threadGetPointer(num);
		tm_syslog(TM_STR("\t%i %s\n"), num, mod_t->name);
	}*/

	/* *************************************************************************************************
	 * End of test and experimental zone
	 * ************************************************************************************************/

	// Start scheduler
	tm_syslog(TM_STR("[info] start round-robin scheduler\n"));

	tm_softwareTimeCounterInit();

	tm_schedulerInit();
	tm_schedulerStart();

	panic(TM_STR("\nsystem launch fail"));

	return 0; // You should never get here
}
