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
 * @file TaskMate.c
 * @brief Implements TaskMate preemptive scheduler & RTC.
 *
 * This file contains :
 * - system, drivers and thread initialisation
 *
 * @todo finish run level
 *
 * Now or later I will have to write a very lightweight libc,
 * designed for embedded systems with MCU. Not POSIX compliant.
 *
 */

#include "hal/hal_user_api.h"
#include "hal/hal_system_critical_api.h"
#include "hal/autoInclude_hal_target.h"

#include "sysCore/autoAlloc.h" // get DRIVERS_CONT
#include "sysCore/modules.h"
#include "sysCore/initSys.h"
#include "sysCore/runLevel.h"
#include "libc/stdio.h"


int main(void)
{
	// system static allocation init
	initDrivers();
	initThreads();
	runLevelInit();

	// hal hardware init
	hal_archInit();
	hal_mcuInit();
	hal_boardInit();

	// start driver
	// todo remove this code when run level is implemented

	for( uint8_t i = 0; i < DRIVERS_COUNT; i++ )
	{
		module_item_driver_t *mod_d = moduleDriverGetPointer(i);
		(*(mod_d->init))();
		(*(mod_d->start))();
	}

	// log test
	// char format[] = "Hello <%c> <%s> <%i> <%x> <%b> <%%> <%w> ";
	char dest[128];
	char test[] = "string test";
	uint16_t i = 1023;

	snprintf(dest, 128, "[%s] \n\nBoot ...\n", __FILE__);
	hal_usartWriteString(dest);
	hal_usartSendTXBuffer();

	snprintf(dest, 128, "[%s] taget : %s-%s-%s\n", __FILE__, target.arch, target.mcu, target.board);
	hal_usartWriteString(dest);
	hal_usartSendTXBuffer();

	snprintf(dest, 128, "Hello <%c> <%s> <%i> <%x> <%b> <%%> <%w> \n", '@', test, i, i, i, i, i);
	hal_usartWriteString(dest);
	hal_usartSendTXBuffer();

	// rtc time test

	hal_rtc_time_t t;
	t.day = 10;
	t.hours = 22;
	t.minutes = 47;
	t.month = 12;
	t.seconds = 15;
	t.weekday = 3;
	t.year = 25;

	hal_ZS_042Write(&t);

	hal_ZS_042Read(&t);

	snprintf(dest, 128, "[time] %i/%i/20%i %i:%i\n", t.day, t.month, t.year, t.hours, t.minutes);
	hal_usartWriteString(dest);
	hal_usartSendTXBuffer();

	// jump to current thread for first call and start system by enabling INT
	moduleThreadSetCurrent(0);
	module_item_thread_t *mod_t = moduleThreadGetPointer(moduleThreadGetCurrent());
	hal_setStackPointer((uintptr_t)mod_t->stack_pointer);

	hal_contextRestore();
	hal_setGlobalInterupt();
	hal_returnFromInterupt();

	return 0; // You should never get here
}
