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
 * @file string.c
 * @brief implementation of libc string
 *
 */

#include "libc/string.h"

void strncpy(char *dest, uint8_t n, const char *src)
{
	uint8_t i = 0;

	while( (i < n) && (src[i] != 0) && (i < 255) )
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;
}
