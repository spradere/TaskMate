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
 * @file tm_snprintf.h
 * @brief header of libc formatted print in string
 *
 */

#ifndef TM_SNPRINTF_H
#define TM_SNPRINTF_H

#include <stdarg.h>
#include <stdint.h>

int tm_snprintf(char *buff, uint8_t buff_size, const char *format, ...);
int tm_vsnprintf(char *buff, uint8_t buff_size, const char *format, va_list args);

#endif
