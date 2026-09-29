/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file targetWireSignal.c
 * @brief FreeBSD simulation GPIO wiring implementation.
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
	if( signal == GPIO_SIGNAL_TASK1_LED )
	{
		signal_names[signal] = "task1";
		return;
	}
	if( signal == GPIO_SIGNAL_TASK2_LED ) { signal_names[signal] = "task2"; }
}
