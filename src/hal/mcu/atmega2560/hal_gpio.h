/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file hal_gpio.h
 * @brief header hal gpio implementation
 *
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>
#include "sysCall/gpio.h"

void hal_gpioInitPin(const gpio_pin_item_t *pin);
void hal_gpioWritePin(const gpio_pin_item_t *pin, bool value);
bool hal_gpioReadPin(const gpio_pin_item_t *pin);

#endif
