/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file boot.c
 * @brief boot implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <stdint.h>

#include "interfaces/drv_usart.h"
#include "interfaces/hal_halt.h"
#include "interfaces/tm_info.h"
#include "system/sysCore/sys_modules.h"
#include "system/sysCore/sys_scheduler.h"
#include "system/sysCore/sys_softwareTimeCounter.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

int main(void)
{
	// Usart startup
	hal_driver_state_t state = hal_usartControl(DRV_CTRL_INIT, 0);
	if( state != DRV_STATE_INITIALIZED){hal_halt();}
	state = hal_usartControl(DRV_CTRL_START, 0);
	if( state != DRV_STATE_RUNNING){hal_halt();}

	hal_usartWriteByte('\n');	
	hal_usartWriteByte('1');
	hal_usartSendTXBuffer();
			
	// Initialise static system allocations
	mod_driversAlloc();
	mod_threadsAlloc();

	hal_usartWriteByte('2');
	hal_usartSendTXBuffer();
	
	// Start scheduler
	tm_softwareTimeCounterInit();

	hal_usartWriteByte('3');
	hal_usartWriteByte('\n');	
	hal_usartSendTXBuffer();

	tm_schedulerInit();
	tm_schedulerStart();

	hal_halt();
}
