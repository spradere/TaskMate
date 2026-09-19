/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_context.h
 * @brief Portable opaque execution context declaration.
 */

#ifndef INTERFACES_HAL_CONTEXT_H
#define INTERFACES_HAL_CONTEXT_H

/* ============================================================================
 * Public definitions
 * ========================================================================== */

typedef struct hal_context hal_context_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

/** Initialize a context on the final word of architecture-defined stack storage. */
void hal_threadContextInit(void (*func)(void), hal_context_t *context, void *stack_top);

/** Restore and start a context as one indivisible architecture operation. */
_Noreturn void hal_contextStart(const hal_context_t *context);

#endif // INTERFACES_HAL_CONTEXT_H
