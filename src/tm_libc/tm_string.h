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
 * @file tm_string.h
 * @brief header of libc string
 *
 */

#ifndef TM_STRING_H
#define TM_STRING_H

#include "hal/auto_hal_define.h" // get libc selection

#if TM_LIBC_CSTD
	#include <string.h>
	#define tm_strncpy strncpy
#endif

#if TM_LIBC_TASKMATE
	#include <stdint.h>
char *tm_strncpy(char *dest, const char *src, uint8_t n);
#endif

#endif
