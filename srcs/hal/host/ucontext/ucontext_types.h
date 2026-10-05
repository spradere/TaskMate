/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file ucontext_types.h
 * @brief FreeBSD ucontext host type declarations.
 */

#ifndef UCONTEXT_UCONTEXT_TYPES_H
#define UCONTEXT_UCONTEXT_TYPES_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>
#include <ucontext.h>

#include "interfaces/hal_context.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @name Host thread stack sizes
 * @brief Reserve ucontext stack words and boundary canaries.
 * @{
 */
#define UCONTEXTTYPES_COUNT_STACKWORD 32768U
#define UCONTEXTTYPES_COUNT_CANARYWORD 2U
/** @} */

typedef uint8_t hal_stack_word_t;

/**
 * @brief Holds the native host context for a suspended thread.
 */
struct hal_context
{
	ucontext_t *native;
};

#endif // UCONTEXT_UCONTEXT_TYPES_H
