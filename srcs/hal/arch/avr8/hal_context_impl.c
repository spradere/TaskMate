/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_context_impl.c
 * @brief avr8 hal_context_impl implementation.
 *
 */

#include "hal/arch/avr8/hal_context_impl.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "interfaces/modules_define.h"

// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

void hal_threadContextInit(void (*func)(void), hal_stack_word_t **stack_pointer,
						   hal_stack_word_t *stack_top)
{
	// stack init
	hal_stack_word_t *sp = stack_top;

	*(sp--) = (uint8_t)((uintptr_t)func & 0xFF); // PCL;
	*(sp--) = (uint8_t)(((uintptr_t)func >> 8u) & 0xFF); // PCH
	*(sp--) = 0x00; // PCHH is always 0 if flash code size < 128k
	*(sp--) = 0x00; // R0
	*(sp--) = SREG;

	// registers R1-R31
	for( int i = 1; i < AVR8_REGISTER_COUNT; i++ ) { *(sp--) = 0x00; }

	*stack_pointer = sp;
}
// NOLINTEND
