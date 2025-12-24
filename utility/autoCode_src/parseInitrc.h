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
 * @file parseInitrc.h
 * @brief read init.rc file header
 *
 */

#ifndef PARSEINITRC_H
#define PARSEINITRC_H

#include "autoCode.h"

void parseInitrc(int TYPE, modules_database_t *data_base, const char *file_initrc_name);

#endif
