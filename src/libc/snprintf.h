/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file snprintf.h
 * @brief header of libc formated print in string
 *
 */

#ifndef SNPRINTF_H
#define SNPRINTF_H

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

void snprintf(char *buff, size_t buff_size, const char *format, ...);

#endif
