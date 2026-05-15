/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_boardInit.h
 * @brief arduinoMega hal_boardInit header declarations.
 *
 */

#ifndef HAL_BOARDINIT_H
#define HAL_BOARDINIT_H

#include "mcu/atmega2560/hal_gpio_impl.h"
#include "interfaces/gpio_signals.h"

void hal_boardInit(void);
void hal_boardWireSignal(hal_signal_t *table, gpio_signal_t signal);

#endif
