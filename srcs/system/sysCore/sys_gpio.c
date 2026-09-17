/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sys_gpio.c
 * @brief gpio implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "sys_gpio.h"

#include "hal/public/hal_gpio.h"
#include "system/sysCore/sys_hal_init.h"
#include "interfaces/hal_targetWireSignal.h"

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static hal_signal_t signal_table[GPIO_SIGNAL_COUNT];

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void gpio_signalsInit(void)
{
	for( uint8_t i = 0; i < GPIO_SIGNAL_COUNT; i++ )
	{
		hal_targetWireSignal(signal_table, i);
		hal_gpioPinInit(&signal_table[i].pin);
	}
}

void gpio_signalSet(gpio_signal_t signal, bool val)
{
	hal_gpioPinWrite(signal_table[signal].pin, val);
}
bool gpio_signalGet(gpio_signal_t signal) { return hal_gpioPinRead(signal_table[signal].pin); }
