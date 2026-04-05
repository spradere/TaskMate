/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD 2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file error.h
 * @brief sysCall error header declarations.
 *
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdint.h>

#include "interfaces/auto_error_catalog.h"
#include "interfaces/tm_string_storage.h"

typedef struct
{
	const tm_string_t *name;
	const err_critical_t critical;
} err_item_t;

const tm_string_t *err_getMessage(uint8_t num);

#endif
