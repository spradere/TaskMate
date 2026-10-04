/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file at2560_gpio.h
 * @brief gpio header declarations.
 *
 */

#ifndef ATMEGA2560_AT2560_GPIO_H
#define ATMEGA2560_AT2560_GPIO_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>

#include "hal/mcu/atmega2560/at2560_ports_pins.h"
#include "interfaces/gpio_signals.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Maps a logical signal to a pin and active level.
 */
typedef struct
{
	hal_pin_t pin;
	bool active_high;
} hal_signal_t;

/**
 * @brief Groups GPIO register addresses for one port.
 */
typedef struct
{
	volatile uint8_t *ddr;
	volatile uint8_t *port;
	volatile uint8_t *pin;
} hal_port_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Configure a physical GPIO pin.
 */
void hal_gpioPinInit(const hal_pin_t *pin);
/**
 * @brief Set a physical GPIO pin level.
 */
void hal_gpioPinWrite(const hal_pin_t pin, bool value);
/**
 * @brief Read a physical GPIO pin level.
 * @return Current electrical level of the pin.
 */
bool hal_gpioPinRead(const hal_pin_t pin);

#endif // ATMEGA2560_AT2560_GPIO_H
