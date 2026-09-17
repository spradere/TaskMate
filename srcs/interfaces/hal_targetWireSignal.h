/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_tagetWireSignal.h
 * @brief Generic target wire signal.
 */

#ifndef INTERFACES_TAGET_WIRE_SIGNAL_H
#define INTERFACES_TAGET_WIRE_SIGNAL_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>
#include <avr/io.h>

#include "hal/public/hal_gpio.h"
#include "interfaces/gpio_signals.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

void hal_targetWireSignal(hal_signal_t *table, gpio_signal_t signal);

#endif // INTERFACES_TAGET_WIRE_SIGNAL_H
