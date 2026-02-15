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
 * @file writeInclude.h
 * @brief writeInclude header
 *
 */

#ifndef WRITE_INCLUDE_H
#define WRITE_INCLUDE_H

#include "autoCode.h"
#include "options.h"

typedef enum
{
	INCLUDE_THREAD_LIST,
	INCLUDE_HAL_USER_PART,
	INCLUDE_HAL_SYSTEM_PART,
	INCLUDE_HAL_DEFINE,
	INCLUDE_HAL_INIT,
	INCLUDE_HAL_STDIO,

	INCLUDE_TYPE_COUNT
} include_type_t;

void writeInclude(const modules_database_t *data_base, include_type_t type ,const char *file_name, const options_list_t *auto_options);

#endif
