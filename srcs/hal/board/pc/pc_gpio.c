/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file pc_gpio.c
 * @brief PC virtual GPIO implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "pc_gpio.h"

#include <stdbool.h>
#include <stdint.h>

#include "interfaces/hal_atomic.h"
#include "pc_console.h"

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static const char *signal_names[GPIO_SIGNAL_COUNT];
static bool signal_values[GPIO_SIGNAL_COUNT];

// [autoCode_tag] wire_gpio
#include "wire_gpio.inc"
// [/tag]

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void hal_gpioSignalInit(void)
{
	hal_atomic_state_t state = hal_atomicStart();
	for( uint8_t i = 0; i < GPIO_SIGNAL_COUNT; i++ )
	{
		signal_names[i] = "unwired";
		targetWireSignal((gpio_signal_t)i);
		signal_values[i] = false;
		pc_consoleLedWrite(i, signal_names[i], false);
	}
	hal_atomicEnd(state);
}

void hal_gpioSignalWrite(gpio_signal_t signal, bool value)
{
	if( signal >= GPIO_SIGNAL_COUNT ) { return; }
	hal_atomic_state_t state = hal_atomicStart();
	signal_values[signal] = value;
	pc_consoleLedWrite((uint8_t)signal, signal_names[signal], value);
	hal_atomicEnd(state);
}

bool hal_gpioSignalRead(gpio_signal_t signal)
{
	if( signal >= GPIO_SIGNAL_COUNT ) { return false; }
	return signal_values[signal];
}
