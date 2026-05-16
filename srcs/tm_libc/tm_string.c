/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_string.c
 * @brief tm_libc tm_string implementation.
 *
 */

#include "tm_libc/tm_string.h"

#include "interfaces/tm_string_storage.h"

void tm_strncpy(char *dest, const char *src, uint8_t n)
{
	uint8_t i = 0;

	while( (i < n) && (src[i] != 0) && (i < TM_STRING_SIZE_MAX) )
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;
}
