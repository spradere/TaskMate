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
 * @todo finish run level : add init service for startup
 *
 * Now or later I will have to write a very lightweight libc,
 * designed for embedded systems with MCU. Not POSIX compliant.
 *
 *
 * sysCallPreemptProtected(timeout, driver);
 */

#include "sysCore/autoInclude.h"
#include "sysCore/autoAlloc.h"

#include "sysCore/modules_items.h"

#include "sysCore/initSys.h"
#include "sysCore/runLevel.h"


// system core wide variables
modules_t modules;

int main(void)
{
	// hal hardware init
	hal_archInit();
	hal_mcuInit();
	hal_boardInit();

	// system static allocation init
	initDrivers();
	initThreads();
	runLevelInit();

	// start driver
	for( uint8_t i = 0; i < DRIVERS_COUNT; i++ )
	{
		(*modules.drivers[i].init)();
		(*modules.drivers[i].start)();
	}

	// jump to current thread for first call and start system by enabling INT
	modules.thread_current = 0;

	hal_setStackPointer((uintptr_t)modules.threads[modules.thread_current].stack_pointer);

	hal_contextRestore();
	hal_setGlobalInterupt();
	hal_returnFromInterupt();

	return 0; // You should never get here
}





