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

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <stdint.h>

#include "interfaces/drv_usart.h"
#include "interfaces/hal_halt.h"
#include "interfaces/tm_info.h"
#include "interfaces/tm_macros.h"
#include "interfaces/tm_modules.h"
#include "system/sysCore/sys_modules.h"
#include "system/sysCore/sys_scheduler.h"
#include "system/sysCore/sys_softwareTimeCounter.h"
#include "tmLibc/tm_syslog.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

int main(void)
{
	// System startup
	hal_usartControl(DRV_CTRL_INIT, 0);
	hal_usartControl(DRV_CTRL_START, 0);

	tm_syslog(TM_STR("\n\n[boot] System startup ...\n"));

	// Initialise static system allocations
	tm_syslog(TM_STR("[boot] system static allocation\n"));

	mod_driversAlloc();
	mod_threadsAlloc();

	tm_syslog(
		TM_STR("[boot] %s v%i.%i build : %i\n"), __FILE__, TM_VER_MAJOR, TM_VER_MINOR, TM_BUILD);

	// Start scheduler
	tm_syslog(TM_STR("[boot] start round-robin scheduler\n"));

	tm_softwareTimeCounterInit();

	tm_schedulerInit();
	tm_schedulerStart();

	hal_halt();
}
