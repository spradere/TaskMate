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
 * @brief Implements system init
 *
 * @todo finish run level
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

int main(void)
{
	// log start
	char log[128];

	hal_usartInit();
	hal_usartStart();

	tm_snprintf(log, sizeof(log), "\n\n[%s] Boot ...\n", __FILE__);
	hal_usartWriteString(log);
	hal_usartSendTXBuffer();

	const sc_target_info_t *target;
	sc_targetGetInfo(&target);
	tm_snprintf(log, sizeof(log), "[boot] target : %s-%s-%s\n", target->arch, target->mcu, target->board);
	hal_usartWriteString(log);
	hal_usartSendTXBuffer();

	// system static allocation init
	tm_snprintf(log, sizeof(log), "[boot] system static allocation\n");
	hal_usartWriteString(log);
	hal_usartSendTXBuffer();

	mod_driversAlloc();
	mod_threadsAlloc();
	rl_Alloc();

	// hal hardware init
	tm_snprintf(log, sizeof(log), "[boot] hal hardware init\n");
	hal_usartWriteString(log);
	hal_usartSendTXBuffer();

	hal_archInit();
	hal_mcuInit();
	hal_boardInit();

	// start driver
	// todo remove this code when run level is implemented

	for( uint8_t i = 0; i < MODULES_DRIVER_COUNT; i++ )
	{
		mod_driver_item_t *mod = mod_driverGetPointer(i);
		(*(mod->init))();
		(*(mod->start))();
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

	// hal_ZS_042Write(&t);
	// hal_ZS_042Read(&t);

	tm_snprintf(log, sizeof(log), "[time test] %i/%i/20%i %i:%i\n", t.day, t.month, t.year, t.hours,
				t.minutes);
	hal_usartWriteString(log);
	hal_usartSendTXBuffer();

	// jump to current thread for first call and start system by enabling INT
	tm_snprintf(log, sizeof(log), "[boot] start round-robin scheduler\n");
	hal_usartWriteString(log);
	hal_usartSendTXBuffer();

	mod_threadSetCurrent(0);
	mod_thread_item_t *mod = mod_threadGetPointer(mod_threadGetCurrent());
	hal_setStackPointer((uintptr_t)mod->stack_pointer);

	hal_contextRestore();
	hal_setGlobalInterupt();
	hal_returnFromInterupt();

	return 0; // You should never get here
}
