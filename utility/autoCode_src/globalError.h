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
 * @file global_error.h
 * @brief global error header
 *
 */

#ifndef GLOBAL_ERROR_H
#define GLOBAL_ERROR_H

#include "utility/autoCode_src/autoCode.h"

typedef enum
{
	ERROR_NOT_DEFINED,
	ERROR_LOW,
	ERROR_MID,
	ERROR_HIGH
}error_critical_t;

typedef struct
{
	char name[256];
	char message[256];
	error_critical_t critical;
} error_item_t;

typedef struct
{
	error_item_t catalog[256];
	int error_count;
}error_catalog_t;

void globalError(const char *file_in_name, error_catalog_t *errors);

#endif
