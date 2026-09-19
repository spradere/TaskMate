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

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void gpio_signalsInit(void) { hal_gpioSignalsInit(); }

void gpio_signalSet(gpio_signal_t signal, bool val) { hal_gpioSignalWrite(signal, val); }
bool gpio_signalGet(gpio_signal_t signal) { return hal_gpioSignalRead(signal); }
