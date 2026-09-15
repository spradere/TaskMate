/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file avr8_define.h
 * @brief arch define header declarations.
 *
 */

#ifndef HAL_ARCH_AVR8_AVR8_DEFINE_H
#define HAL_ARCH_AVR8_AVR8_DEFINE_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>
#include <avr/pgmspace.h>

#include "interfaces/tm_macros.h"
#include "interfaces/tm_string.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

#define TM_STR_NEW(name, txt) \
	static const char TM_UNIQUE_NAME(name)[] PROGMEM = (txt); \
	static const tm_string_t (name) = {.text = TM_UNIQUE_NAME(name), .storage = TM_MEM_ROM}

#define TM_STR_ROM(string) ((tm_string_t){ .text = PSTR(string), .storage = TM_MEM_ROM })
#define TM_STR_RAM(string) ((tm_string_t){ .text = (string), .storage = TM_MEM_RAM })

// Default storage for AVR8
#define TM_STR(string) TM_STR_ROM(string)

/* -----------------------------------------------
 * Architecture constants
 * ---------------------------------------------*/

#define AVR8_REGISTER_COUNT 32 // from R0 to R31

/* -----------------------------------------------
 * Architecture types
 * ---------------------------------------------*/

typedef uint8_t hal_stack_word_t;
typedef uint8_t hal_atomic_state_t;

typedef struct
{
	volatile uint8_t *ddr;
	volatile uint8_t *port;
	volatile uint8_t *pin;
} hal_port_t;

#endif // HAL_ARCH_AVR8_AVR8_DEFINE_H
