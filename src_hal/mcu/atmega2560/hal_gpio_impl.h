/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_gpio_impl.h
 * @brief atmega2560 hal_gpio_impl header declarations.
 *
 */

#ifndef HAL_GPIO_IMPL_H
#define HAL_GPIO_IMPL_H

#include <stdbool.h>

#include "mcu/atmega2560/hal_mcu_define.h"

typedef struct
{
	hal_pin_t pin;
	bool active_high;
} hal_signal_t;

void hal_gpioPinInit(const hal_pin_t *pin);
void hal_gpioPinWrite(const hal_pin_t pin, bool value);
bool hal_gpioPinRead(const hal_pin_t pin);

#endif
