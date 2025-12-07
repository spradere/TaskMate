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
 * @file gpio.h
 * @brief gpio header
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>
#include "hal/autoInclude_hal_target.h" // get : gpio_port_index_t gpio_signal_t

typedef enum
{
	GPIO_PIN_MODE_INPUT,
	GPIO_PIN_MODE_OUTPUT_PP,	// push-pull
	GPIO_PIN_MODE_OUTPUT_OD,	// open-drain
} gpio_pin_mode_t;

typedef enum
{
	GPIO_PIN_PULL_NONE,
	GPIO_PIN_PULL_UP,
	GPIO_PIN_PULL_DOWN
} gpio_pin_pull_t;

typedef struct
{
	gpio_port_index_t port_index;
	uint8_t	number;
	gpio_pin_mode_t	mode;
	gpio_pin_pull_t	pull;
} gpio_pin_item_t;

typedef struct
{
	gpio_pin_item_t pin;
	bool active_high;
}gpio_signal_item_t;

void gpioSignalInit(gpio_signal_t signal, gpio_signal_item_t *sig);
void gpioSignalSet(gpio_signal_t signal, bool on);
bool gpioSignalGet(gpio_signal_t signal);
void gpioSignalToggle(gpio_signal_t signal);

#endif
