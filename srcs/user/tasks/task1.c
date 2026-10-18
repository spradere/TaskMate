/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file task1.c
 * @brief task1 implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "task1.h"

#include "system/sysCall/sc_gpio.h"
#include "system/sysCall/sc_threads.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define TASK1_DELAY_TICKS 50U

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void task1(void)
{
	sc_threadSetInitialized();

	while( 1U )
	{

		sc_gpio_signalToggle(GPIO_SIGNAL_TASK1_LED);

		sc_threadSetSTC(TASK1_DELAY_TICKS);
		while( sc_threadGetSTC() > 0U );
	}
}
