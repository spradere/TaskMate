/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file error.h
 * @brief System wide error header
 *
 */

#ifndef ERROR_H
#define ERROR_H

#include "sysCall/auto_error_catalog.h"
#include "tm_libc/tm_string.h"

const tm_string_t *err_getMessage(uint8_t num);

#endif
