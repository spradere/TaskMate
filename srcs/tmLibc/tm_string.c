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

#if TM_LIBC_TASKMATE
/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

int strncmp(tm_string_t left, tm_string_t right, size_t n)
{
	for( size_t i = 0U; i < n; i++ )
	{
		uint8_t left_byte = sc_stringGetByte(&left, i);
		uint8_t right_byte = sc_stringGetByte(&right, i);
		if( left_byte != right_byte ) { return (int)left_byte - (int)right_byte; }
		if( left_byte == 0U ) { return 0U; }
	}
	return 0U;
}

char *strncpy(char *dest, tm_string_t src, size_t n)
{
	size_t i = 0U;
	while( i < n )
	{
		uint8_t value = sc_stringGetByte(&src, i);
		dest[i++] = (char)value;
		if( value == 0U ) { break; }
	}
	while( i < n ) { dest[i++] = 0U; }
	return dest;
}
#endif
