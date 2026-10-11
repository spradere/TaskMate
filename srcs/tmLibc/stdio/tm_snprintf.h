/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_snprintf.h
 * @brief tm snprintf header declarations.
 *
 */

#ifndef STDIO_TM_SNPRINTF_H
#define STDIO_TM_SNPRINTF_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "tmLibc/tm_string.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

// clang-format off
#if TM_LIBC_TASKMATE
	#define printf taskmate_printf
	#define vprintf taskmate_vprintf
	#define snprintf taskmate_snprintf
	#define vsnprintf taskmate_vsnprintf

/* -----------------------------------------------
 * Stream formatting
 * ---------------------------------------------*/

/**
 * @brief Format text and write it to the console.
 * @return Number of formatted bytes.
 */
int printf(tm_string_t format, ...);
/**
 * @brief Format argument-list text and write it to the console.
 * @return Number of formatted bytes.
 */
int vprintf(tm_string_t format, va_list args);

/* -----------------------------------------------
 * Bounded buffer formatting
 * ---------------------------------------------*/

/**
 * @brief Format text into a bounded RAM buffer.
 * @param[out] ptr Destination buffer.
 * @param size Destination capacity in bytes.
 * @param format Storage-aware format string.
 * @return Number of bytes that would be written, excluding the final NUL.
 */
int snprintf(char *ptr, size_t size, tm_string_t format, ...);
/**
 * @brief Format argument-list text into a bounded RAM buffer.
 * @param[out] ptr Destination buffer.
 * @param size Destination capacity in bytes.
 * @param format Storage-aware format string.
 * @param args Arguments referenced by the format string.
 * @return Number of bytes that would be written, excluding the final NUL.
 */
int vsnprintf(char *ptr, size_t size, tm_string_t format, va_list args);

#endif
// clang-format on

#endif // STDIO_TM_SNPRINTF_H
