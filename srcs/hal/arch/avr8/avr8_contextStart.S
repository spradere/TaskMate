/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file avr8_contextStart.S
 * @brief AVR8 initial context restore implementation.
 */

#include <avr/io.h>

#include "avr8_context.inc"

/* =============================================================================
 * Context start
 * ========================================================================== */

.section .text.hal_contextStart, "ax", @progbits
.global hal_contextStart
.type hal_contextStart, @function

hal_contextStart:
	/* The AVR-GCC ABI passes the context pointer in r25:r24. */
	movw r30, r24
	ld r26, Z+
	ld r27, Z
	out AVR_STACK_POINTER_HI_ADDR, r27
	out AVR_STACK_POINTER_LO_ADDR, r26
	AVR8_CONTEXT_RESTORE
	reti

.size hal_contextStart, .-hal_contextStart
