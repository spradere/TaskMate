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
 * @file gpio.c
 * @brief gpio implementation
 */

#include "sysCall/gpio.h"

#include "hal/auto_hal_user.h"

void gpio_signalsInit(void)
{
	for( uint8_t i = 0; i < GPIO_SIGNAL_COUNT; i++ ) { hal_gpioWireSignal(i); }
}

void gpio_signalSet(gpio_signal_t signal, bool val) { hal_gpioWritePin(signal, val); }
bool gpio_signalGet(gpio_signal_t signal) { return hal_gpioReadPin(signal); }
void gpio_signalToggle(gpio_signal_t signal) { gpio_signalSet(signal, !gpio_signalGet(signal)); }
