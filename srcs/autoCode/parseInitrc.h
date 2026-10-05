/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file parseInitrc.h
 * @brief parse initrc header declarations.
 *
 */

#ifndef AUTOCODE_PARSEINITRC_H
#define AUTOCODE_PARSEINITRC_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include "autoCode.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Parse an init.rc file into the module database.
 * @param[in,out] data_base Receives parsed modules and commands.
 * @param initrc_name init.rc file path.
 * @param source_path Base path for referenced sources.
 * @return AC_RESULT_OK on file success; AC_RESULT_ERROR on file failure.
 */
ac_result_t parseInitrc(modules_database_t *data_base, const char *initrc_name,
						const char *source_path);

#endif // AUTOCODE_PARSEINITRC_H
