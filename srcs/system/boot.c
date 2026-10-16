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

#include "interfaces/tm_driver_have.h"
#ifdef TM_DRIVER_HAVE_GPIO
	#include "interfaces/drv_gpio.h"
#endif
#include "interfaces/drv_usart.h"
#include "interfaces/hal_halt.h"
#include "interfaces/tm_info.h"
#include "system/sysCore/sys_drivers.h"
#include "system/sysCore/sys_scheduler.h"
#include "system/sysCore/sys_threads.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

int main(void)
{
	hal_driver_state_t state;
#ifdef TM_DRIVER_HAVE_GPIO
	// GPIO startup precedes USART and static driver allocation.
	state = hal_gpioControl(DRV_CTRL_INIT, 0U);
	if( state != DRV_STATE_INITIALIZED ) { hal_halt(); }
	state = hal_gpioControl(DRV_CTRL_START, 0U);
	if( state != DRV_STATE_RUNNING ) { hal_halt(); }
#endif
	// Usart startup
	state = hal_usartControl(DRV_CTRL_INIT, 0U);
	if( state != DRV_STATE_INITIALIZED ) { hal_halt(); }
	state = hal_usartControl(DRV_CTRL_START, 0U);
	if( state != DRV_STATE_RUNNING ) { hal_halt(); }

	hal_usartWriteByte('\n');
	hal_usartWriteByte('1');
	hal_usartSendTXBuffer();

	// Initialise static system allocations
	mod_driversAlloc();
	mod_threadsAlloc();

	hal_usartWriteByte('2');
	hal_usartSendTXBuffer();

	// Start scheduler
	hal_usartWriteByte('3');
	hal_usartWriteByte('\n');
	hal_usartSendTXBuffer();

	tm_schedulerInit();
	tm_schedulerStart();

	hal_halt();
}
