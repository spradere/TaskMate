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

#include "interfaces/tm_options.h"
#include "interfaces/tm_cpp_compat.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Default limit for bounded application text operations.
 */
#define TM_STRING_SIZE_MAX 255

/**
 * @brief Identifies whether a string resides in RAM or ROM.
 */
#if TM_LIBC_TASKMATE
typedef enum __attribute__((packed))
{
	TM_MEM_RAM,
	TM_MEM_ROM
} tm_string_storage_t;
TM_STATIC_ASSERT(sizeof(tm_string_storage_t) == 1U, "tm_string_storage_t must be one byte");

/**
 * @brief Identifies string data and its RAM or ROM storage.
 */
typedef struct
{
	const uint8_t *text;
	const tm_string_storage_t storage;
} tm_string_t;
typedef tm_string_t tm_string_arg_t;
#else
typedef char tm_string_t;
typedef const char *tm_string_arg_t;
#endif

#endif // INTERFACES_TM_STRING_H
