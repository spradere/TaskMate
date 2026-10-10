/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file ucontext_context.c
 * @brief ucontext execution-context implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "ucontext_context.h"

#include <stddef.h>
#include <stdint.h>
#include <ucontext.h>

#include "hal/host/freebsd/freebsd_interrupts.h"
#include "hal/host/freebsd/freebsd_timerSched.h"
#include "interfaces/hal_halt.h"
#include "interfaces/tm_mod.h"
#include "ucontext_types.h"

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static ucontext_t initial_contexts[MOD_THREAD_COUNT];
static mod_count_t initial_context_count;

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void hal_threadContextInit(void (*func)(void), hal_context_t *context, void *stack_top)
{
	if( (func == NULL) || (context == NULL) || (stack_top == NULL) ) { hal_halt(); }
	if( initial_context_count >= MOD_THREAD_COUNT ) { hal_halt(); }
	context->native = &initial_contexts[initial_context_count++];
	if( getcontext(context->native) != 0U ) { hal_halt(); }

	/*
	 * This experimental target gives every generated thread exactly the same
	 * stack size. stack_top points to the final usable word, between the two
	 * sysCore canaries.
	 */
	const size_t stack_size =
		(UCONTEXTTYPES_COUNT_STACKWORD - UCONTEXTTYPES_COUNT_CANARYWORD) *
		sizeof(hal_stack_word_t);
	uint8_t *stack_end = (uint8_t *)stack_top + sizeof(hal_stack_word_t);
	context->native->uc_stack.ss_sp = stack_end - stack_size;
	context->native->uc_stack.ss_size = stack_size;
	context->native->uc_stack.ss_flags = 0U;
	context->native->uc_link = 0U;
	if( sigemptyset(&context->native->uc_sigmask) != 0U ) { hal_halt(); }
	makecontext(context->native, func, 0U);
}

_Noreturn void hal_contextStart(const hal_context_t *context)
{
	if( context == NULL ) { hal_halt(); }
	/* setcontext() restores the initial empty mask after this indivisible start. */
	if( !freebsd_interruptsBlock(0U) ) { hal_halt(); }
	freebsd_timerSchedActivate();
	if( setcontext(context->native) != 0U ) { hal_halt(); }
	hal_halt();
}
