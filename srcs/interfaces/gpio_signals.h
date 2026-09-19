/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file gpio_signals.h
 * @brief gpio signals header declarations.
 *
 */

#ifndef INTERFACES_GPIO_SIGNALS_H
#define INTERFACES_GPIO_SIGNALS_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>

/* ============================================================================
 * Public definitions
 * ========================================================================== */

// [autoCode_tag] gpio_signals
#include "gpio_signals.inc"
// [/tag]

void hal_gpioSignalsInit(void);
void hal_gpioSignalWrite(gpio_signal_t signal, bool value);
bool hal_gpioSignalRead(gpio_signal_t signal);

#endif // INTERFACES_GPIO_SIGNALS_H
