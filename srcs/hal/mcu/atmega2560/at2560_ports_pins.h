/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file at2560_ports_pins.h
 * @brief ATmega2560 port and pin type declarations.
 *
 */

#ifndef HAL_MCU_ATMEGA2560_AT2560_PORTS_PINS_H
#define HAL_MCU_ATMEGA2560_AT2560_PORTS_PINS_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

#include "interfaces/gpio_common.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

typedef enum
{
	PORT_A,
	PORT_B,
	PORT_C,
	PORT_D,
	PORT_E,
	PORT_F,
	PORT_G,
	PORT_H,
	PORT_I,
	PORT_J,
	PORT_K,
	PORT_L,

	PORT_COUNT
} hal_port_list_t;

typedef struct
{
	hal_port_list_t port;
	uint8_t number;
	gpio_pin_mode_t mode;
	gpio_pin_pull_t pull;
} hal_pin_t;

#endif // HAL_MCU_ATMEGA2560_AT2560_PORTS_PINS_H
