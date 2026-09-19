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

#include "hal/public/hal_ports_pins.h"
#include "interfaces/gpio_signals.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

typedef struct
{
	hal_pin_t pin;
	bool active_high;
} hal_signal_t;

typedef struct
{
	volatile uint8_t *ddr;
	volatile uint8_t *port;
	volatile uint8_t *pin;
} hal_port_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

void hal_gpioPinInit(const hal_pin_t *pin);
void hal_gpioPinWrite(const hal_pin_t pin, bool value);
bool hal_gpioPinRead(const hal_pin_t pin);



#endif // ATMEGA2560_AT2560_GPIO_H
