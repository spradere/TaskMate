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
 * @file mcu_define.h
 * @brief atmega2560 mcu_define header declarations.
 *
 */

#ifndef MCU_DEFINE_H
#define MCU_DEFINE_H

#include <stdint.h>

#include "interfaces/gpio_commons.h"

// frequencies
#define USART_BAUD_RATE 9600
#define I2C_FREQ 100000UL // Standard mode 100 kHz

// atmega2560 ports
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

// atmega2560 pin structure
typedef struct
{
	hal_port_list_t port;
	uint8_t number;
	gpio_pin_mode_t mode;
	gpio_pin_pull_t pull;
} hal_pin_t;

#endif
