/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file ucontext_types.h
 * @brief FreeBSD ucontext target type declarations.
 */

#ifndef HAL_TARGET_UCONTEXT_UCONTEXT_TYPES_H
#define HAL_TARGET_UCONTEXT_UCONTEXT_TYPES_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>
#include <ucontext.h>

#include "interfaces/hal_context.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

#define UCONTEXT_THREAD_STACK_WORD_COUNT 32768u
#define UCONTEXT_THREAD_STACK_CANARY_WORD_COUNT 2u

typedef uint8_t hal_stack_word_t;

struct hal_context
{
	ucontext_t *native;
};

#endif // HAL_TARGET_UCONTEXT_UCONTEXT_TYPES_H
