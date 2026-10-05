/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file parseTag.h
 * @brief parse tag header declarations.
 *
 */

#ifndef AUTOCODE_PARSETAG_H
#define AUTOCODE_PARSETAG_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include "autoCode.h"
#include "globalError.h"
#include "options.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Reset tag parser state before processing source files.
 */
void parseTagInit(void);
/**
 * @brief Process tagged regions in a source file.
 * @param[in,out] data_base Parsed module data.
 * @param file_name Tagged source path.
 * @param errors Parsed error catalogue.
 * @param auto_options Code generation options.
 * @return AC_RESULT_OK on success; AC_RESULT_ERROR on file failure.
 */
ac_result_t parseTag(modules_database_t *data_base, const char *file_name,
				 const error_catalog_t *errors,
			 const options_list_t *auto_options);
/**
 * @brief Report which supported tags were found.
 */
void parseTagHave(void);

#endif // AUTOCODE_PARSETAG_H
