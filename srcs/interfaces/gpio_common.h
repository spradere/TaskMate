/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file gpio_common.h
 * @brief gpio common header declarations.
 */

#ifndef INTERFACES_GPIO_COMMON_H
#define INTERFACES_GPIO_COMMON_H

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Selects a physical GPIO pin mode.
 */
typedef enum __attribute__((packed))
{
	GPIO_PIN_MODE_INPUT,
	GPIO_PIN_MODE_OUTPUT_PP, // push-pull
	GPIO_PIN_MODE_OUTPUT_OD, // open-drain
	GPIO_PIN_MODE_OUTPUT_HZ, // high impedance
} gpio_pin_mode_t;
_Static_assert(sizeof(gpio_pin_mode_t) == 1U, "gpio_pin_mode_t must be one byte");

/**
 * @brief Selects a physical GPIO pin pull configuration.
 */
typedef enum __attribute__((packed))
{
	GPIO_PIN_PULL_NONE,
	GPIO_PIN_PULL_UP,
	GPIO_PIN_PULL_DOWN
} gpio_pin_pull_t;
_Static_assert(sizeof(gpio_pin_pull_t) == 1U, "gpio_pin_pull_t must be one byte");

#endif // INTERFACES_GPIO_COMMON_H
