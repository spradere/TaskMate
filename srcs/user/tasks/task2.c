/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file task2.c
 * @brief task2 implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "task2.h"

#include "system/sysCall/sc_errors.h"
#include "system/sysCall/sc_gpio.h"
#include "system/sysCall/sc_threads.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define TASK2_DELAY_TICKS 50U

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void task2(void)
{
	if( !sc_threadSetInitialized() ) { sc_halt(); }
	uint16_t ticks;

	while( 1U )
	{
		sc_gpio_signalToggle(GPIO_SIGNAL_TASK2_LED);

		if( !sc_threadSetSTC(TASK2_DELAY_TICKS) ) { sc_halt(); }
		do
		{
			if( !sc_threadGetSTC(&ticks) ) { sc_halt(); }
		} while( ticks > 0U );
	}
}
