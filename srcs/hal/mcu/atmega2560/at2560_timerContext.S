/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file at2560_timerContext.S
 * @brief ATmega2560 timer context-switch interrupt implementation.
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#include "hal/arch/avr8/avr8_context.inc"
#include "hal/mcu/atmega2560/at2560_timerContext.inc"

/* =============================================================================
 * Private data
 * ========================================================================== */

.section .bss.scheduler_context, "aw", @nobits
.type scheduler_context, @object

scheduler_context:
	.zero 2

.size scheduler_context, .-scheduler_context

/* =============================================================================
 * Timer clock control
 * ========================================================================== */

.section .text.asm_timerContextClockStart, "ax", @progbits
.global asm_timerContextClockStart
.type asm_timerContextClockStart, @function

asm_timerContextClockStart:
	AT2560_TIMERCONTEXT_START
	ret

.size asm_timerContextClockStart, .-asm_timerContextClockStart

.section .text.asm_timerContextClockStop, "ax", @progbits
.global asm_timerContextClockStop
.type asm_timerContextClockStop, @function

asm_timerContextClockStop:
	AT2560_TIMERCONTEXT_STOP
	ret

.size asm_timerContextClockStop, .-asm_timerContextClockStop

/* =============================================================================
 * Context-switch interrupt
 * ========================================================================== */

.section .text.TIMER1_COMPA_vect, "ax", @progbits
.global TIMER1_COMPA_vect
.type TIMER1_COMPA_vect, @function

TIMER1_COMPA_vect:
	AVR8_CONTEXT_SAVE
	AT2560_TIMERCONTEXT_STOP
	in r18, AVR_STACK_POINTER_LO_ADDR
	in r19, AVR_STACK_POINTER_HI_ADDR
	sts scheduler_context, r18
	sts scheduler_context+1, r19
	ldi r24, lo8(scheduler_context)
	ldi r25, hi8(scheduler_context)
	lds r30, schedCallback
	lds r31, schedCallback+1
	sbiw r30, 0
	breq .Lcallback_done
	icall

.Lcallback_done:
	movw r30, r24
	ld r18, Z+
	ld r19, Z
	out AVR_STACK_POINTER_HI_ADDR, r19
	out AVR_STACK_POINTER_LO_ADDR, r18
	AT2560_TIMERCONTEXT_START
	AVR8_CONTEXT_RESTORE
	reti

.size TIMER1_COMPA_vect, .-TIMER1_COMPA_vect
