/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file stack.h
 * @brief avr8 hal_stack_impl header declarations.
 *
 */

#ifndef AVR8_STACK_H
#define AVR8_STACK_H

#include <avr/io.h> // get SP
#include <stdint.h>

#include "hal/arch/avr8/arch_define.h"

static inline __attribute__((always_inline)) void hal_setStackPointer(hal_stack_word_t *sp)
{
	SP = (uintptr_t)sp;
}
static inline __attribute__((always_inline)) hal_stack_word_t *hal_getStackPointer(void)
{
	return (hal_stack_word_t *)SP;
}

#endif // AVR8_STACK_H
