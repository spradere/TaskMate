/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file initrcCommand.h
 * @brief Shared init.rc command parser declarations.
 */

#ifndef AUTOCODE_INITRCCOMMANDS_INITRCCOMMAND_H
#define AUTOCODE_INITRCCOMMANDS_INITRCCOMMAND_H

/* =============================================================================
 * Includes
 * ===========================================================================*/

#include "../autoCode.h"
#include "../tokenizer.h"

/* =============================================================================
 * Public definitions
 * ===========================================================================*/

typedef struct
{
	modules_database_t *data_base;
	const tokenizer_t *tok;
	const char *initrc_name;
	const char *source_path;
	int file_line_number;
} initrc_command_t;

#endif // AUTOCODE_INITRCCOMMANDS_INITRCCOMMAND_H
