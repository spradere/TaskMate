/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file hal_threadContextinit.c
 * @brief hal thread context init
 *
 */

#include "hal/arch/avr8/hal_context.h"

#include <avr/io.h>

#include "sysCore/modules_define.h"
#include "tm_libc/tm_syslog.h"

// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

void hal_threadContextInit(void (*func)(void), hal_stack_word_t **stack_pointer,
						   hal_stack_word_t *stack_top)
{
	// stack init
	hal_stack_word_t *sp = stack_top;

	tm_syslog(TM_STR("[hal_context]  func = %04x\n"), func);
	tm_syslog(
		TM_STR("[hal_context] top *sp = %04x bottom = %04x\n"), sp, (sp)-MOD_THREAD_STACK_SIZE);

	*(sp--) = (uint8_t)((uintptr_t)func & 0xFF); // PCL;
	*(sp--) = (uint8_t)(((uintptr_t)func >> 8u) & 0xFF); // PCH
	*(sp--) = 0x00; // PCHH is always 0 if flash code size < 128k
	*(sp--) = 0x00; // R0
	*(sp--) = SREG;

	// registers R1-R31
	for( int i = 1; i < AVR8_REGISTER_COUNT; i++ ) { *(sp--) = 0x00; }

	*stack_pointer = sp;
	tm_syslog(TM_STR("[hal_context] after write *sp = %04x \n"), sp);

	sp = stack_top;
	tm_syslog(TM_STR("[hal_context]  PCH:PCL = %02x%02x \n\n"), *(sp - 1), *(sp));
}
// NOLINTEND
