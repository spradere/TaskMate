/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_cpp_compat.h
 * @brief Language-neutral assertion and non-returning declaration forms.
 */

#ifndef INTERFACES_TM_CPP_COMPAT_H
#define INTERFACES_TM_CPP_COMPAT_H

/* ============================================================================
 * Public definitions
 * ========================================================================== */

#ifdef __cplusplus
	#define TM_STATIC_ASSERT(condition, message) static_assert(condition, message)
	#define TM_NORETURN [[noreturn]]
#else
	#define TM_STATIC_ASSERT(condition, message) _Static_assert(condition, message)
	#define TM_NORETURN _Noreturn
#endif

#endif // INTERFACES_TM_CPP_COMPAT_H
