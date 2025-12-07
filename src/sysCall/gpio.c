/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file gpio.c
 * @brief gpio implementation
 */

#include "sysCall/TaskMate_public.h"
#include "sysCall/gpio.h"
#include "hal/hal_user_api.h"

static gpio_signal_item_t gpio_signals_table[GPIO_SIGNAL_COUNT];

void gpioSignalInit(gpio_signal_t signal, gpio_signal_item_t *sig)
{
	gpio_signals_table[signal] = *sig;
	hal_gpioInitPin(&(sig->pin));
}

void gpioSignalSet(gpio_signal_t signal, bool on)
{
	bool val = gpio_signals_table[signal].active_high ? on : !on;
	hal_gpioWritePin(&(gpio_signals_table[signal].pin), val);
}

bool gpioSignalGet(gpio_signal_t signal)
{
	bool val = hal_gpioReadPin(&(gpio_signals_table[signal].pin));
	return gpio_signals_table[signal].active_high ? val : !val;
}

void gpioSignalToggle(gpio_signal_t signal) { gpioSignalSet(signal, !gpioSignalGet(signal)); }
