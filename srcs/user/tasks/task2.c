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

#include "system/sysCall/sc_gpio.h"
#include "system/sysCall/sc_threads.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define TASK2_DELAY_STC_TICKS 50U

/* -----------------------------------------------
 * Task state
 * ---------------------------------------------*/

uint8_t task2_msg_channel;

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void task2(void)
{
	sc_threadSetInitialized();

	while( 1U )
	{
		sc_gpio_signalToggle(GPIO_SIGNAL_TASK2_LED);

		sc_threadSetSTC(TASK2_DELAY_STC_TICKS);
		while( sc_threadGetSTC() > 0U );
	}
}
