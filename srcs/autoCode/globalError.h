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
#define ERROR_COUNT_MAX 256

/**
 * @brief Stores one parsed error declaration.
 */
typedef struct
{
	char name[AC_BUFFER_SIZE];
	char message[AC_BUFFER_SIZE];
	err_level_t level;
} error_item_t;

/**
 * @brief Holds the parsed error catalogue.
 */
typedef struct
{
	error_item_t catalog[ERROR_COUNT_MAX];
	int error_count;
} error_catalog_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Parse an error catalogue into the supplied storage.
 * @param src_name Error catalogue path.
 * @param[out] errors Receives parsed error definitions.
 * @return Zero on success; a negative value on file failure.
 */
int globalError(const char *src_name, error_catalog_t *errors);

#endif // AUTOCODE_GLOBALERROR_H
