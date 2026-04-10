/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_arch_define.h
 * @brief avr8 arch_define header declarations.
 *
 */

#ifndef ARCH_DEFINE_H
#define ARCH_DEFINE_H

#include <stdint.h>

// AVR arch definition
#define AVR8_REGISTER_COUNT 32 // from R0 to R31

// stack related size
typedef uint8_t hal_stack_word_t;
typedef uint8_t hal_irq_sate_t;

// avr8 port structure
typedef struct
{
	volatile uint8_t *ddr;
	volatile uint8_t *port;
	volatile uint8_t *pin;
} hal_port_t;

#endif
