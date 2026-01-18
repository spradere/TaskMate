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
 * @file tm_string.c
 * @brief implementation of libc string
 *
 */

#include "tm_libc/tm_string.h"

char *tm_strncpy(char *dest, const char *src, uint8_t n)
{
	uint8_t i = 0;

	// todo remove magic number 255, replace with TM_STRING_SIZE_MAX
	while( (i < n) && (src[i] != 0) && (i < 255) )
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;
}
