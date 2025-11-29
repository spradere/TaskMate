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

#include "sysCore/autoAlloc.h" // get DRIVERS_CONT
#include "sysCore/modules.h"

#include "sysCore/initSys.h"
#include "sysCore/runLevel.h"

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
	module_item_driver_t *mod_d;

	for( uint8_t i = 0; i < DRIVERS_COUNT; i++ )
	{
		mod_d = moduleDriverGetPointer(i);
		(*(mod_d->init))();
		(*(mod_d->start))();
	}

	// jump to current thread for first call and start system by enabling INT
	moduleThreadSetCurrent(0);
	module_item_thread_t *mod_t = moduleThreadGetPointer(moduleThreadGetCurrent());
	hal_setStackPointer((uintptr_t)mod_t->stack_pointer);

	hal_contextRestore();
	hal_setGlobalInterupt();
	hal_returnFromInterupt();

	return 0; // You should never get here
}
