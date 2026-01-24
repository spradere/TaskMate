/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file option.h
 * @brief autoCode option parser header
 *
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#include "autoCode.h"
#include "fileUtility.h"

// autoCode option structure
typedef struct
{
	char arch_name[BYTE_INDEX];
	char mcu_name[BYTE_INDEX];
	char board_name[BYTE_INDEX];
} options_list_t;

typedef struct
{
	const char *name;
	void (*func)(const char *value, options_list_t *opt);

} options_cmd_t;

void options(const char *file_name, options_list_t *opt);

#endif
