/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file avr8_stack.h
 * @brief stack header declarations.
 *
 */

#ifndef AVR8_AVR8_STACK_H
#define AVR8_AVR8_STACK_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <avr/io.h> // Get SP
#include <stdint.h>

#include "hal/public/hal_architecture_types.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

static inline __attribute__((always_inline)) void hal_setStackPointer(const hal_context_t *context)
{
	SP = (uintptr_t)context->stack_pointer;
}
#endif // AVR8_AVR8_STACK_H
