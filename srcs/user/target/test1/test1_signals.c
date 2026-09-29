/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tagetWireSignal.c
 * @brief target wire signal implementation.
 */

/* ============================================================================
 * Includes
 * ========================================================================== */

#include "interfaces/gpio_signals.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static void targetWireSignal(gpio_signal_t signal)
{
	// Set default values for outputs
	signal_table[signal].pin.mode = GPIO_PIN_MODE_OUTPUT_PP;
	signal_table[signal].pin.pull = GPIO_PIN_PULL_NONE;
	signal_table[signal].active_high = true;

	if( signal == GPIO_SIGNAL_INBOARD_LED )
	{
		signal_table[signal].pin.port = PORT_B;
		signal_table[signal].pin.number = PB7;
		return;
	}

	if( signal == GPIO_SIGNAL_TASK1_LED )
	{
		signal_table[signal].pin.port = PORT_A;
		signal_table[signal].pin.number = PA0;
		return;
	}

	if( signal == GPIO_SIGNAL_TASK2_LED )
	{
		signal_table[signal].pin.port = PORT_A;
		signal_table[signal].pin.number = PA1;
		return;
	}
}
