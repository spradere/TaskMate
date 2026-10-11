/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_string.h
 * @brief tm string header declarations.
 *
 */

#ifndef TMLIBC_TM_STRING_H
#define TMLIBC_TM_STRING_H

/* ============================================================================
 * Target selection
 * ========================================================================== */

// clang-format off

#include "interfaces/tm_options.h" // get libc selection
#include "interfaces/tm_string.h"

#if TM_LIBC_CSTD
	#include <string.h>
#endif

#if TM_LIBC_TASKMATE
	#include <stddef.h>
	#include "system/sysCall/sc_string.h"
	#define strncmp taskmate_strncmp
	#define strncpy taskmate_strncpy

/**
 * @brief Compare up to n bytes of storage-aware strings.
 * @param n Maximum number of bytes to compare.
 * @return Zero if equal within the limit; negative or positive for ordering.
 */
	int strncmp(tm_string_t left, tm_string_t right, size_t n);
/**
 * @brief Copy and NUL-pad exactly n bytes from a storage-aware string.
 * @param[out] dest Destination RAM buffer.
 * @param n Maximum number of bytes to copy.
 * @return Destination buffer.
 */
	char *strncpy(char *dest, tm_string_t src, size_t n);
#endif

// clang-format on

#endif // TMLIBC_TM_STRING_H
