/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sc_string.h
 * @brief String syscall declarations.
 *
 */

#ifndef SYSCALL_SC_STRING_H
#define SYSCALL_SC_STRING_H

// clang-format off

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <avr/pgmspace.h>
#include <stdint.h>

#include "interfaces/tm_macros.h"
#include "interfaces/tm_string.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

#define TM_STR_ROM_NEW(name, txt) \
	static const char TM_UNIQUE_NAME(name)[] PROGMEM = (txt); \
	static const tm_string_t (name) = {.text = TM_UNIQUE_NAME(name), .storage = TM_MEM_ROM}

#define TM_STR_ROM(string) ((tm_string_t){ .text = PSTR(string), .storage = TM_MEM_ROM })
#define TM_STR_RAM(string) ((tm_string_t){ .text = (string), .storage = TM_MEM_RAM })

// Default storage for AVR8
#define TM_STR(string) TM_STR_ROM(string)

/* ============================================================================
 * Public API
 * ========================================================================== */

void sc_stdioPutChar(char ch);
uint8_t sc_stringGetByte(const tm_string_t *string, uint8_t index);
int sc_stringCompare(tm_string_t left, tm_string_t right, uint8_t size);
void sc_stringCopy(char *dest, tm_string_t src, uint8_t size);

// clang-format on

#endif // SYSCALL_SC_STRING_H
