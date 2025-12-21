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
 * @file parseTag.h
 * @brief read tag header
 *
 */

#ifndef PARSETAG_H
#define PARSETAG_H

#include "autoCode.h"
#include "globalError.h"

void parseTag(modules_database_t *data_base, const char *file_name, error_catalog_t *errors, const target_t *target);

#endif
