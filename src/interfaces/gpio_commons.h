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
 * @file gpio_commons.h
 * @brief interfaces gpio_commons header declarations.
 */

#ifndef GPIO_COMMONS_H
#define GPIO_COMMONS_H

typedef enum
{
	GPIO_PIN_MODE_INPUT,
	GPIO_PIN_MODE_OUTPUT_PP, // push-pull
	GPIO_PIN_MODE_OUTPUT_OD, // open-drain
	GPIO_PIN_MODE_OUTPUT_HZ, // high impedance
} gpio_pin_mode_t;

typedef enum
{
	GPIO_PIN_PULL_NONE,
	GPIO_PIN_PULL_UP,
	GPIO_PIN_PULL_DOWN
} gpio_pin_pull_t;

#endif
