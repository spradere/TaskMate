/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file writeInclude.h
 * @brief autoCode writeInclude header declarations.
 *
 */

#ifndef WRITEINCLUDE_H
#define WRITEINCLUDE_H

#include "autoCode.h"
#include "options.h"

typedef enum
{
	INCLUDE_MODULES_LIST,
	INCLUDE_HAL_DEFINE,
	INCLUDE_HAL_INIT,

	INCLUDE_TYPE_COUNT
} include_type_t;

void writeInclude(const modules_database_t *data_base, include_type_t type ,const char *file_name, const options_list_t *auto_options);

#endif
