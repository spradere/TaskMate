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

#include <stdint.h>

#include "hal/autoInclude_hal_system_critical.h"
#include "hal/autoInclude_hal_user.h"

#include "sysCore/modules.h"
#include "sysCore/initSys.h"
#include "sysCore/runLevel.h"
#include "libc/stdio.h"
#include "sysCall/sysCall.h"

int main(void)
{
	// log start
	char log[128];

	hal_usartInit();
	hal_usartStart();

	snprintf(log, sizeof(log), "\n\n[%s] Boot ...\n", __FILE__);
	hal_usartWriteString(log);
	hal_usartSendTXBuffer();

	const target_info_t *target;
	sysCallGetTargetInfo(&target);
	snprintf(log, sizeof(log), "[boot] target : %s-%s-%s\n", target->arch, target->mcu, target->board);
	hal_usartWriteString(log);
	hal_usartSendTXBuffer();

	// system static allocation init
	snprintf(log, sizeof(log), "[boot] system static allocation\n");
	hal_usartWriteString(log);
	hal_usartSendTXBuffer();

	initDrivers();
	initThreads();
	runLevelInit();

	// hal hardware init
	snprintf(log, 128, "[boot] hal hardware init\n");
	hal_usartWriteString(log);
	hal_usartSendTXBuffer();

	hal_archInit();
	hal_mcuInit();
	hal_boardInit();

	// start driver
	// todo remove this code when run level is implemented

	for( uint8_t i = 0; i < MODULES_DRIVER_COUNT; i++ )
	{
		module_item_driver_t *mod_d = moduleDriverGetPointer(i);
		(*(mod_d->init))();
		(*(mod_d->start))();
	}

	// rtc time test

	hal_rtc_time_t t;
	t.day = 17;
	t.hours = 20;
	t.minutes = 47;
	t.month = 12;
	t.seconds = 15;
	t.weekday = 3;
	t.year = 25;

	//hal_ZS_042Write(&t);
	//hal_ZS_042Read(&t);

	snprintf(log, sizeof(log), "[time test] %i/%i/20%i %i:%i\n", t.day, t.month, t.year, t.hours, t.minutes);
	hal_usartWriteString(log);
	hal_usartSendTXBuffer();

	// jump to current thread for first call and start system by enabling INT
	snprintf(log, sizeof(log), "[boot] start round-robin scheduler\n");
	hal_usartWriteString(log);
	hal_usartSendTXBuffer();

	moduleThreadSetCurrent(0);
	module_item_thread_t *mod_t = moduleThreadGetPointer(moduleThreadGetCurrent());
	hal_setStackPointer((uintptr_t)mod_t->stack_pointer);

	hal_contextRestore();
	hal_setGlobalInterupt();
	hal_returnFromInterupt();

	return 0; // You should never get here
}
