/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file options.h
 * @brief options header declarations.
 *
 */

#ifndef AUTOCODE_OPTIONS_H
#define AUTOCODE_OPTIONS_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include "autoCode.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Holds autoCode input and output paths and test mode.
 */
typedef struct
{
	bool test_mode;
	char file_errors_list[AUTOCODE_SIZE_BUFFER];
	char file_initrc_list[AUTOCODE_SIZE_BUFFER];
	char file_parsetag_list[AUTOCODE_SIZE_BUFFER];
	char file_gpio_signals[AUTOCODE_SIZE_BUFFER];
	char file_wire_gpio[AUTOCODE_SIZE_BUFFER];
	char generated_path[AUTOCODE_SIZE_BUFFER];
	char source_path[AUTOCODE_SIZE_BUFFER];

} options_list_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Read autoCode options from a configuration file.
 * @param file_name Options file path.
 * @param[out] opt Receives parsed paths and flags.
 * @return AC_RESULT_OK on success; AC_RESULT_ERROR on file failure.
 */
ac_result_t options(const char *file_name, options_list_t *opt);

#endif // AUTOCODE_OPTIONS_H
