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
 * @file gpio.h
 * @brief sysCore gpio header declarations.
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>
#include <stdint.h>

#include "interfaces/gpio_signals.h"

void gpio_signalsInit(void);
void gpio_signalSet(gpio_signal_t signal, bool val);
bool gpio_signalGet(gpio_signal_t signal);
void gpio_signalToggle(gpio_signal_t signal);

#endif
