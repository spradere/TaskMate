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
 * @file sc_gpio.c
 * @brief sysCall sc_gpio implementation.
 */

#include "sysCall/sc_gpio.h"

#include "sysCore/gpio.h"

void sc_gpio_signalSet(gpio_signal_t signal, bool val) { gpio_signalSet(signal, val); }
bool sc_gpio_signalGet(gpio_signal_t signal) { return gpio_signalGet(signal); }
void sc_gpio_signalToggle(gpio_signal_t signal) { gpio_signalSet(signal, !gpio_signalGet(signal)); }
