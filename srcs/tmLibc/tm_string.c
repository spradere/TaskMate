/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_string.c
 * @brief tm string implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "tm_string.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

int tm_strncmp(tm_string_t left, tm_string_t right, uint8_t n)
{
	return sc_stringCompare(left, right, n);
}

void tm_strncpy(char *dest, tm_string_t src, uint8_t n) { sc_stringCopy(dest, src, n); }
