/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file avr8_context.c
 * @brief context implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "avr8_context.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "avr8_architecture_constants.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void hal_threadContextInit(void (*func)(void), hal_context_t *context, void *stack_top)
{
	// Stack initialisation
	hal_stack_word_t *sp = stack_top;

	*(sp--) = (uint8_t)((uintptr_t)func & 0xFF); // PCL;
	*(sp--) = (uint8_t)(((uintptr_t)func >> 8u) & 0xFF); // PCH
	*(sp--) = 0x00; // PCHH is always 0 if the flash code size is below 128 kB
	*(sp--) = 0x00; // R0
	*(sp--) = SREG;

	// Registers R1-R31
	for( uint8_t i = 1; i < AVR8_REGISTER_COUNT; i++ ) { *(sp--) = 0x00; }

	context->stack_pointer = sp;
}

_Noreturn __attribute__((naked)) void hal_contextStart(const hal_context_t *)
{
	asm volatile("movw r30, r24 \n\t"
				 "ld r26, Z+ \n\t"
				 "ld r27, Z \n\t"
				 "out __SP_H__, r27 \n\t"
				 "out __SP_L__, r26 \n\t"
				 AVR8_CONTEXT_RESTORE
				 "sei \n\t"
				 "reti \n\t");
	__builtin_unreachable();
}
