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

#include <arv/pgmsapce.h>
#include <stdint.h>

// macro for function wrapper
#define CONCAT2(a,b) a##b
#define CONCAT(a,b) CONCAT2(a,b)
#define UNIQUE_NAME(prefix) CONCAT(prefix, __LINE__)

#define tm_vsnprintf(ptr, size, format, vargs) \
	tm_string_t UNIQUE_NAME(str) = { \
		.text = PSRT(format), \
		.len = 256, \
		.storage = STRING_ROM}; \
		return tm_vsnprintf_function(ptr, size, UNIQUE_NAME(str), (vargs))

// tm_sting implemetation
typedef enum
{
	STRING_RAM,
	STRING_ROM
}tm_string_storage_t;

typedef struct
{
	const char *text;
	uint8_t len;
	const tm_string_storage_t storage;
}tm_string_t;

// functions
void hal_stdio_putChar(char ch);
char hal_string_getChar(tm_string_t *str, uint8_t index);

#endif
