/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sc_gpio.h
 * @brief sc gpio header declarations.
 */

#ifndef SYSCALL_SC_GPIO_H
#define SYSCALL_SC_GPIO_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <stdint.h>

#include "interfaces/gpio_signals.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Initialize logical GPIO signal mappings.
 */
void sc_gpio_signalInit(void);
/**
 * @brief Set a logical GPIO signal.
 */
void sc_gpio_signalSet(gpio_signal_t signal, bool val);
/**
 * @brief Read a logical GPIO signal.
 * @return Current logical value of the signal.
 */
bool sc_gpio_signalGet(gpio_signal_t signal);
/**
 * @brief Toggle a logical GPIO signal.
 */
void sc_gpio_signalToggle(gpio_signal_t signal);

#endif // SYSCALL_SC_GPIO_H
