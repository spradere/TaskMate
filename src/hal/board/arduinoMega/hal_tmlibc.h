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
 * @file hal_tmlibc.h
 * @brief board stdio header
 *
 */

#ifndef HAL_TMLIBC_H
#define HAL_TMLIBC_H

// clang-format off

#include <avr/pgmspace.h>
#include <stdint.h>

#include "tm_libc/tm_string.h"

// macro for in file scope string storage
#define CONCAT2(a,b) a##b
#define CONCAT(a,b) CONCAT2(a,b)
#define UNIQUE_NAME(prefix) CONCAT(prefix, __LINE__)

#define TM_STR_ROM_NEW(name, txt) \
	static const char UNIQUE_NAME(name)[] PROGMEM = (txt); \
	static const tm_string_t (name) = {.text = UNIQUE_NAME(name), .storage = TM_MEM_ROM}

// macro for in function string storage
#define TM_STR_ROM(string) ((tm_string_t){ .text = PSTR(string), .storage = TM_MEM_ROM })
#define TM_STR_RAM(string) ((tm_string_t){ .text = (string), .storage = TM_MEM_RAM })
// default storage for avr8
#define TM_STR(string) TM_STR_ROM(string)

// functions
void hal_stdio_putChar(char ch);
char hal_string_getChar(const tm_string_t *str, uint8_t index);

// clang-format on
#endif
