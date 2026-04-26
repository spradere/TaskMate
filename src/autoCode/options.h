/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file options.h
 * @brief autoCode options header declarations.
 *
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#include "autoCode.h"
#include "fileUtility.h"

// autoCode option structure
typedef struct
{
	char tm_ver[BYTE_INDEX];
	char tm_build[BYTE_INDEX];
	char arch_name[BYTE_INDEX];
	char mcu_name[BYTE_INDEX];
	char board_name[BYTE_INDEX];
	char file_errors_list[BYTE_INDEX];
	char file_initrc_list[BYTE_INDEX];
	char file_parsetag_list[BYTE_INDEX];

} options_list_t;

void options(const char *file_name, options_list_t *opt);

#endif
