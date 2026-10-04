/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_stdio.h
 * @brief tm stdio header declarations.
 *
 */

#ifndef TMLIBC_TM_STDIO_H
#define TMLIBC_TM_STDIO_H

/* ============================================================================
 * Target selection
 * ========================================================================== */

// clang-format off

#include "interfaces/tm_options.h" // get libc selection

#if TM_LIBC_CSTD
	#include <stdio.h>
/**
 * @name Standard formatting aliases
 * @brief Route formatting operations to the standard C library.
 * @{
 */
	#define tm_snprintf snprintf
	#define tm_printf printf
/** @} */
#endif

#if TM_LIBC_TASKMATE
	#include "tmLibc/stdio/tm_snprintf.h"
#endif

// clang-format on

#endif // TMLIBC_TM_STDIO_H
