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
 * @file printf.c
 * @brief implementation of libc print formated
 *
 * @todo nothing
 */

#include <avr/io.h>
#include <stdarg.h>

// private function
void vsnprintf(char *buf, uint8_t n, const char *format, va_list args)
{}

// public function
void snprintf(char *buf, uint8_t n, const char *format, ...)
{}
