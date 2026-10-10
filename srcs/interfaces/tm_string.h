/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_string.h
 * @brief tm string storage header declarations.
 *
 */

#ifndef INTERFACES_TM_STRING_H
#define INTERFACES_TM_STRING_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Maximum length used by storage-aware string operations.
 */
#define TM_STRING_SIZE_MAX 255

/**
 * @brief Identifies whether a string resides in RAM or ROM.
 */
typedef enum __attribute__((packed))
{
	TM_MEM_RAM,
	TM_MEM_ROM
} tm_string_storage_t;
_Static_assert(sizeof(tm_string_storage_t) == 1U, "tm_string_storage_t must be one byte");

/**
 * @brief Identifies string data and its RAM or ROM storage.
 */
typedef struct
{
	const uint8_t *text;
	const tm_string_storage_t storage;
} tm_string_t;

#endif // INTERFACES_TM_STRING_H
