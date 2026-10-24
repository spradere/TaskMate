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

#include <avr/interrupt.h>
#include <avr/io.h>

#include "hal/arch/avr8/avr8_types.h"

/* -----------------------------------------------
 * Private Constants
 * ---------------------------------------------*/

#define AVR8CONTEXT_MASK_PC 0xFFU
#define AVR8CONTEXT_SHIFT_PCH 8U
#define AVR8CONTEXT_R_COUNT 30U // r register count without r0 and r1

/* -----------------------------------------------
 * Private structs
 * ---------------------------------------------*/

typedef struct __attribute__((packed))
{

	uint8_t r_registers[AVR8CONTEXT_R_COUNT];
	uint8_t r1;
	uint8_t avr8_RAMPZ;
	uint8_t avr8_SREG;
	uint8_t r0;

	uint8_t pc_ext;
	uint8_t pc_high;
	uint8_t pc_low;

} avr8__context_t;

_Static_assert(sizeof(avr8__context_t) == 37, "Invalid AVR8 context size");

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void hal_threadContextInit(void (*func)(void), hal_context_t *context, void *stack_top)
{
	// Stack initialisation
	hal_stack_word_t *stack_word_top = stack_top;
	avr8__context_t *ctx = (avr8__context_t *)(stack_word_top - sizeof(avr8__context_t));

	// set special register value
	ctx->pc_low = (uint8_t)((uintptr_t)func & AVR8CONTEXT_MASK_PC);
	ctx->pc_high = (uint8_t)(((uintptr_t)func >> AVR8CONTEXT_SHIFT_PCH) & AVR8CONTEXT_MASK_PC);
	ctx->pc_ext = 0U; // avrgcc ABI handle adress > 16 bytes via trampolines, EIND is out of context
					  // save/restore.

	ctx->r0 = 0U;
	ctx->avr8_SREG = 0U; // I bit is handled by int/reti
	ctx->r1 = 0U; // avrgcc C ABI

	ctx->avr8_RAMPZ = 0U;

	// clear registers R2-R31
	for( uint8_t i = 0U; i < AVR8CONTEXT_R_COUNT; i++ ) { ctx->r_registers[i] = 0U; }

	// set stack pointer
	context->stack_pointer = (hal_stack_word_t *)ctx - 1;
}
