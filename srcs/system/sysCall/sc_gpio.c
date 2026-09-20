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

#include "interfaces/gpio_signals.h"
#include "interfaces/hal_atomic.h"
#include "system/sysCore/sys_modules.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define SC_GPIO_SYSTEM_THREAD_ID 0u

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void sc_gpio_signalInit(void)
{
	if( mod_threadGetCurrent() != SC_GPIO_SYSTEM_THREAD_ID ) { return; }

	hal_gpioSignalInit();
}

void sc_gpio_signalSet(gpio_signal_t signal, bool val) { hal_gpioSignalWrite(signal, val); }
bool sc_gpio_signalGet(gpio_signal_t signal) { return hal_gpioSignalRead(signal); }
void sc_gpio_signalToggle(gpio_signal_t signal)
{
	hal_atomic_state_t state = hal_atomicStart();
	hal_gpioSignalWrite(signal, !hal_gpioSignalRead(signal));
	hal_atomicEnd(state);
}
