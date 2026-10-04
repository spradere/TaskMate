/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file avr8_types.h
 * @brief AVR8 architecture type declarations.
 *
 */

#ifndef AVR8_AVR8_TYPES_H
#define AVR8_AVR8_TYPES_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

#include "interfaces/hal_context.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

typedef uint8_t hal_stack_word_t;

/**
 * @brief Stores the AVR8 stack pointer for a suspended thread.
 */
struct hal_context
{
	hal_stack_word_t *stack_pointer;
};

#endif // AVR8_AVR8_TYPES_H
