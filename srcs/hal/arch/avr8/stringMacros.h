/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file stringMacros.h
 * @brief AVR8 string placement macros.
 */

#ifndef HAL_ARCH_AVR8_STRING_MACROS_H
#define HAL_ARCH_AVR8_STRING_MACROS_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <avr/pgmspace.h>

#include "interfaces/tm_macros.h"
#include "interfaces/tm_string.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

#define TM_STR_NEW(name, txt) \
	static const char TM_UNIQUE_NAME(name)[] PROGMEM = (txt); \
	static const tm_string_t (name) = {.text = TM_UNIQUE_NAME(name), .storage = TM_MEM_ROM}

#define TM_STR(string) ((tm_string_t){.text = PSTR(string), .storage = TM_MEM_ROM})

#endif // HAL_ARCH_AVR8_STRING_MACROS_H
