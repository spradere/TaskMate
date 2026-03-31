/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file hal_gpio_impl.h
 * @brief atmega2560 hal_gpio_impl header declarations.
 *
 */

#ifndef HAL_GPIO_IMPL_H
#define HAL_GPIO_IMPL_H

#include <stdbool.h>

#include "hal/mcu/atmega2560/mcu_define.h"
#include "interfaces/gpio_signals.h"

typedef struct
{
	hal_pin_t pin;
	bool active_high;
} hal_signal_t;

void hal_gpioPinInit(const hal_pin_t *pin);
void hal_gpioWritePin(const hal_pin_t pin, bool value);
bool hal_gpioReadPin(const hal_pin_t pin);

#endif
