/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file error_catalog.h
 * @brief error catalog header declarations.
 */

#ifndef INTERFACES_ERROR_CATALOG_H
#define INTERFACES_ERROR_CATALOG_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Stores one valid error identifier independently of the catalogue count.
 */
typedef uint8_t err_code_t;

#ifdef AUTOCODE_BUILD
/**
 * @brief Provides the placeholder error count during autoCode compilation.
 */
typedef uint8_t error_count_t;
typedef enum __attribute__((packed))
{
	ERROR_COUNT
} err_codes_t;
_Static_assert(sizeof(err_codes_t) <= sizeof(error_count_t),
		   "err_codes_t exceeds error_count_t");
_Static_assert(ERROR_COUNT <= (UINT8_MAX + 1U), "error identifiers exceed err_code_t");
#else
	// [autoCode_tag] error_enum
#include "error_enum.inc"
// [/tag]
#endif

#endif // INTERFACES_ERROR_CATALOG_H
