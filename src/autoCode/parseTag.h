/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file parseTag.h
 * @brief autoCode parseTag header declarations.
 *
 */

#ifndef PARSETAG_H
#define PARSETAG_H

#include "autoCode.h"
#include "globalError.h"
#include "options.h"

void parseTag(modules_database_t *data_base, const char *file_name, const error_catalog_t *errors, const options_list_t *auto_options);

#endif
