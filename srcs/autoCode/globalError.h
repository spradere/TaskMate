/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file globalError.h
 * @brief autoCode globalError header declarations.
 *
 */

#ifndef AUTOCODE_GLOBALERROR_H
#define AUTOCODE_GLOBALERROR_H

#include "autoCode.h"

#define ERROR_COUNT_MAX 256

typedef enum
{
	ERROR_NOT_DEFINED,
	ERROR_LOW,
	ERROR_MID,
	ERROR_HIGH
}error_critical_t;

typedef struct
{
	char name[BYTE_INDEX];
	char message[BYTE_INDEX];
	error_critical_t critical;
} error_item_t;

typedef struct
{
	error_item_t catalog[ERROR_COUNT_MAX];
	int error_count;
}error_catalog_t;

void globalError(const char *src_name, error_catalog_t *errors);

#endif // AUTOCODE_GLOBALERROR_H
