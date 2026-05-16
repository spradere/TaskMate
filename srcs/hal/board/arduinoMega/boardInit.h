/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file boardInit.h
 * @brief arduinoMega hal_boardInit header declarations.
 *
 */

#ifndef ARDUINOMEGA_BOARDINIT_H
#define ARDUINOMEGA_BOARDINIT_H

#include "hal/mcu/atmega2560/gpio.h"
#include "interfaces/gpio_signals.h"

void hal_boardInit(void);
void hal_boardWireSignal(hal_signal_t *table, gpio_signal_t signal);

#endif // ARDUINOMEGA_BOARDINIT_H
