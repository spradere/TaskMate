/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_options.h
 * @brief options header declarations.
 *
 */

#ifndef INTERFACES_TM_OPTIONS_H
#define INTERFACES_TM_OPTIONS_H

#include "interfaces/tm_cpp_compat.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Compile-time diagnostic verbosity level.
 */
#define VERBOSE_LEVEL 0

/**
 * @name C library selection
 * @brief Select exactly one TaskMate or standard C library implementation.
 * @{
 */
#ifndef TM_LIBC_TASKMATE
	#define TM_LIBC_TASKMATE 1
#endif
#ifndef TM_LIBC_CSTD
	#define TM_LIBC_CSTD 0
#endif
/** @} */

#if !(TM_LIBC_CSTD ^ TM_LIBC_TASKMATE)
TM_STATIC_ASSERT(0U, "Select exactly one libc, external or internal");
#endif

#endif // INTERFACES_TM_OPTIONS_H
