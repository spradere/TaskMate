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
 * @file tm_string.h
 * @brief header of libc string
 *
 */

#ifndef TM_STRING_H
#define TM_STRING_H

// clang-format off

#include "TaskMate.h" // get libc selection

// tm_string implementation
typedef enum
{
	TM_MEM_RAM,
	TM_MEM_ROM
}tm_string_storage_t;

typedef struct
{
	const char *text;
	const tm_string_storage_t storage;
}tm_string_t;

#if TM_LIBC_CSTD
	#include <string.h>
	#define tm_strncpy strncpy

	#define TM_STR_RAM(string) (string)
	#define TM_STR_ROM(string) (string)
	#define TM_STR(string) (string)
	#define TM_STR_ROM_NEW(name, txt) \
		const char name[] = (txt);
#endif

#if TM_LIBC_TASKMATE
	#include <stdint.h>
	#include "hal/auto_hal_tmlibc.h" // macro TM_STR_* hal definitions

	void tm_strncpy(char *dest, const char *src, uint8_t n);


#endif

// clang-format on

#endif
