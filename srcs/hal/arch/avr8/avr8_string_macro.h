/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file avr8_string_macro.h
 * @brief AVR8 string macro declarations.
 *
 */

#ifndef AVR8_AVR8_STRING_MACRO_H
#define AVR8_AVR8_STRING_MACRO_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <avr/pgmspace.h>

#include "interfaces/tm_macros.h"
#include "interfaces/tm_string.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @name AVR8 string storage
 * @brief Create string descriptors with AVR8 RAM or program-memory storage.
 * @{
 */
#define HAL_STRING_INROM(name, txt)                           \
	static const uint8_t TM_UNIQUE_NAME(name)[] PROGMEM = (txt); \
	static const tm_string_t(name) = {.text = TM_UNIQUE_NAME(name), .storage = TM_MEM_ROM}

#define HAL_STRING_ROM(string) \
	((tm_string_t){.text = (const uint8_t *)PSTR(string), .storage = TM_MEM_ROM})
#define HAL_STRING_RAM(string) \
	((tm_string_t){.text = (const uint8_t *)(string), .storage = TM_MEM_RAM})

#define HAL_STRING_ROMGETBYTE(ptr) pgm_read_byte(ptr)

// Default storage for AVR8
#define HAL_STRING(string) HAL_STRING_ROM(string)
/** @} */

#endif // AVR8_AVR8_STRING_MACRO_H
