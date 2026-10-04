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
	#define tm_strncmp strncmp
	#define tm_strncpy strncpy
#endif

#if TM_LIBC_TASKMATE
	#include <stdint.h>
	#include "system/sysCall/sc_string.h"

/**
 * @brief Compare up to n bytes of storage-aware strings.
 * @param n Maximum number of bytes to compare.
 * @return Zero if equal within the limit; negative or positive for ordering.
 */
	int tm_strncmp(tm_string_t left, tm_string_t right, uint8_t n);
/**
 * @brief Copy up to n bytes of a storage-aware string to RAM.
 * @param[out] dest Destination RAM buffer.
 * @param n Maximum number of bytes to copy.
 */
	void tm_strncpy(char *dest, tm_string_t src, uint8_t n);
#endif

// clang-format on

#endif // TMLIBC_TM_STRING_H
