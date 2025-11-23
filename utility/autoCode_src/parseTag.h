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
 * @todo nothing
 */

#ifndef PARSETAG_H
#define PARSETAG_H

#include "utility/autoCode_src/autoCode.h"

void parseTag(modules_database_t *data_base, const char *name_src);

static void writeDriversInit(modules_database_t *data_base, FILE *file);
static void writeThreadsInit(modules_database_t *data_base, FILE *file);
static void writeRunLevelsInit(modules_database_t *data_base, FILE *file);

#endif
