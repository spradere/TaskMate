/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file targetInit.h
 * @brief user targetInit header declarations.
 *
 */
 
#include "hal/public/gpio.h"
#include "interfaces/gpio_signals.h"

void targetInit(void);
void targetWireSignal(hal_signal_t *table, gpio_signal_t signal);
