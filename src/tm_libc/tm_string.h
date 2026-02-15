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

#include <stdint.h>

#include "TaskMate.h" // get libc selection

typedef enum
{
	STRING_RAM,
	STRING_ROM
}tm_string_storage_t;

typedef struct
{
	char *text;
	uint8_t len;
	tm_string_storage_t storage;
}tm_string_t;

#if TM_LIBC_CSTD
	#include <string.h>
	#define tm_strncpy strncpy
#endif

#if TM_LIBC_TASKMATE
	#include <stdint.h>
	void tm_strncpy(char *dest, const char *src, uint8_t n);
#endif

// clang-format on

#endif
