/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file globalError.h
 * @brief global error header declarations.
 *
 */

#ifndef AUTOCODE_GLOBALERROR_H
#define AUTOCODE_GLOBALERROR_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include "autoCode.h"
#include "interfaces/error_level.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Maximum entries in a parsed error catalogue.
 */
enum { GLOBALERROR_COUNT_MAX = 256U };

/**
 * @brief Stores one parsed error declaration.
 */
typedef struct
{
	char name[AUTOCODE_SIZE_BUFFER];
	char message[AUTOCODE_SIZE_BUFFER];
	err_level_t level;
} error_item_t;

/**
 * @brief Holds the parsed error catalogue.
 */
typedef struct
{
	error_item_t catalog[GLOBALERROR_COUNT_MAX];
	int error_count;
} error_catalog_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Parse an error catalogue into the supplied storage.
 * @param src_name Error catalogue path.
 * @param[out] errors Receives parsed error definitions.
 * @return AC_RESULT_OK on success; AC_RESULT_ERROR on file failure.
 */
ac_result_t globalError(const char *src_name, error_catalog_t *errors);

#endif // AUTOCODE_GLOBALERROR_H
