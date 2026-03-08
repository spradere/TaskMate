/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file hal_boardInit.h
 * @brief arduino mega board init arduino mega header
 *
 */

#ifndef HAL_BOARDINIT_H
#define HAL_BOARDINIT_H

#include "hal/mcu/atmega2560/hal_gpio.h"
#include "interfaces/gpio_signals.h"

void hal_boardInit(void);
void hal_boardWireSignal( hal_signal_t *table, gpio_signal_t signal);

#endif
