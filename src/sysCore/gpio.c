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
 * @brief sysCore gpio implementation.
 */

#include "sysCore/gpio.h"

#include "hal/public/hal_gpio.h"
#include "sysCore/auto_hal_init.h"

static hal_signal_t signal_table[GPIO_SIGNAL_COUNT];

void gpio_signalsInit(void)
{
	for( uint8_t i = 0; i < GPIO_SIGNAL_COUNT; i++ )
	{
		hal_boardWireSignal(signal_table, i);
		hal_gpioPinInit(&signal_table[i].pin);
	}
}

void gpio_signalSet(gpio_signal_t signal, bool val) { hal_gpioWritePin(signal_table[signal].pin, val); }
bool gpio_signalGet(gpio_signal_t signal) { return hal_gpioReadPin(signal_table[signal].pin); }
void gpio_signalToggle(gpio_signal_t signal) { gpio_signalSet(signal, !gpio_signalGet(signal)); }
