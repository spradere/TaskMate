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
 * @file tm_string_storage.h
 * @brief interfaces tm_string_storage header declarations.
 *
 */

#ifndef TM_STRING_STORAGE_H
#define TM_STRING_STORAGE_H

// tm_string implementation
typedef enum
{
	TM_MEM_RAM,
	TM_MEM_ROM
} tm_string_storage_t;

typedef struct
{
	const char *text;
	const tm_string_storage_t storage;
} tm_string_t;

#endif
