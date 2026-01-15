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
 * @file tm_stdio.h
 * @brief header of libc stdio
 *
 */

#ifndef TM_STDIO_H
#define TM_STDIO_H

#include "hal/auto_hal_define.h" // get libc selection

#if TM_LIBC_CSTD
	#include <stdio.h>
	#define tm_snprintf snprintf
#endif

#if TM_LIBC_TASKMATE
	#include "tm_libc/tm_snprintf.h"
#endif

#endif
