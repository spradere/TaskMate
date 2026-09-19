/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sc_gpio.c
 * @brief sc gpio implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "sc_gpio.h"

#include "hal/mcu/atmega2560/at2560_gpio.h"
#include "system/sysCall/sc_gpio_system.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void sc_gpio_signalsInit(void) { hal_gpioSignalsInit(); }
void sc_gpio_signalSet(gpio_signal_t signal, bool val) { hal_gpioSignalWrite(signal, val); }
bool sc_gpio_signalGet(gpio_signal_t signal) { return hal_gpioSignalRead(signal); }
void sc_gpio_signalToggle(gpio_signal_t signal)
{
	hal_gpioSignalWrite(signal, !hal_gpioSignalRead(signal));
}
