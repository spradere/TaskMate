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
 * @brief Implements system startup
 *
 */

#include "TaskMate.h"

#include <stdint.h>

#include "hal/auto_hal_define.h"
#include "hal/auto_hal_init.h"
#include "hal/auto_hal_system.h"
#include "hal/auto_hal_user.h"
#include "sysCall/error.h"
#include "sysCall/sysCall.h"
#include "sysCore/modules.h"
#include "sysCore/runLevel.h"
#include "tm_libc/tm_stdio.h"
#include "tm_libc/tm_string.h"
#include "tm_libc/tm_syslog.h"

TM_STORE_FILE_NAME;

// display harware target informations
#if VERBOSE_LEVEL > 0
	#pragma message "TM_VERSION  = " TM_VERSION
	#pragma message "ARCH  = " ARCH
	#pragma message "MCU   = " MCU
	#pragma message "BOARD = " BOARD
#endif

int main(void)
{
	// system startup
	hal_usartInit();
	hal_usartStart();

	const sc_info_t *info;
	sc_targetGetInfo(&info);
	tm_syslog(TM_STR("\n\n[boot] TaskMate %s %s boot\n"), &file_name, info->tm_ver);
	tm_syslog(TM_STR("[info] target : %s-%s-%s\n"), info->arch, info->mcu, info->board);

	// system static allocation init
	tm_syslog(TM_STR("[boot] system static allocation\n"));

	mod_driversAlloc();
	mod_threadsAlloc();
	rl_Alloc();

	// hal hardware init
	tm_syslog(TM_STR("[boot] hal hardware init\n"));

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

	// RTC external module test
	tm_syslog(TM_STR("[boot] hal RTC init\n"));
	hal_rtc_time_t t;
	t.hours = 21;
	t.minutes = 41;
	t.seconds = 30;
	t.day = 7;
	t.month = 2;
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
	tm_snprintf(msg, sizeof(msg), TM_STR("TaskMate %s"), info->tm_ver);
	hal_lcdClear();
	hal_lcdSetCursor(0, 0);
	hal_lcdWriteString(msg);

	tm_snprintf(msg,
				sizeof(msg),
				TM_STR("%02i/%02i/20%02i %02i:%02i"),
				t.day,
				t.month,
				t.year,
				t.hours,
				t.minutes);
	hal_lcdSetCursor(1, 0);
	hal_lcdWriteString(msg);

	// test error catalog
	tm_syslog(TM_STR("[info] error catalog\n"));
	for( uint8_t i = 0; i < ERROR_COUNT; i++ )
	{
		const tm_string_t *err_msg = err_getMessage(i);
		tm_syslog(TM_STR("\t%i [0x%04x->0x%04x] <%s>\n"), i, &err_msg, err_msg, err_msg);
	}

	// display module names
	tm_syslog(TM_STR("[modules] drivers\n"));
	mod_driver_item_t *mod_d;

	for( int num = 0; num < MOD_DRIVER_COUNT; num++ )
	{
		mod_d = mod_driverGetPointer(num);
		tm_syslog(TM_STR("\t%i %s\n"), num, mod_d->name);
	}

	tm_syslog(TM_STR("[modules] threads\n"));
	mod_thread_item_t *mod_t;

	for( int num = 0; num < MOD_THREAD_COUNT; num++ )
	{
		mod_t = mod_threadGetPointer(num);
		tm_syslog(TM_STR("\t%i %s\n"), num, mod_t->name);
	}

	// jump to current thread for first call and start system by enabling interrupts
	tm_syslog(TM_STR("[boot] start round-robin scheduler\n"));

	mod_threadSetCurrent(0);
	mod_thread_item_t *mod = mod_threadGetPointer(mod_threadGetCurrent());
	hal_setStackPointer((uintptr_t)mod->stack_pointer);

	hal_contextRestore();
	hal_setGlobalInterupt();
	hal_returnFromInterupt();

	return 0; // You should never get here
}
