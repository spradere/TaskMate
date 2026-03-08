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
 * @file arch_define.h
 * @brief avr8 definitions
 *
 */

#ifndef ARCH_DEFINE_H
#define ARCH_DEFINE_H

#include <stdint.h>

#include "hal/mcu/atmega2560/mcu_define.h"
#include "interfaces/gpio_commons.h"

// AVR arch definition
#define AVR8_REGISTER_COUNT 32 // from R0 to R31

// stack related size
typedef uint8_t hal_stack_word_t;

// avr8 port structure
typedef struct
{
	volatile uint8_t *ddr;
	volatile uint8_t *port;
	volatile uint8_t *pin;
} hal_port_t;

// avr8 pin structure
typedef struct
{
	hal_port_list_t port;
	uint8_t number;
	gpio_pin_mode_t mode;
	gpio_pin_pull_t pull;
} hal_pin_t;



#endif
