/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/*
 * ATmega2560 context-switch timer clock macros for assembly sources.
 */

#ifndef ATMEGA2560_AT2560_TIMERCONTEXT_INC
#define ATMEGA2560_AT2560_TIMERCONTEXT_INC

#include <avr/io.h>

/* =============================================================================
 * Timer clock macros
 * ========================================================================== */

.macro AT2560_TIMERCONTEXT_START
	lds r24, TCCR1B
	/* CS12:CS10 = 010 selects the Timer1 clock divided by 8. */
	ori r24, (1 << CS11)
	sts TCCR1B, r24
.endm

.macro AT2560_TIMERCONTEXT_STOP
	clr r1
	lds r24, TCCR1B
	/* CS12:CS10 = 000 stops Timer1. */
	andi r24, lo8(~((1 << CS12) | (1 << CS11) | (1 << CS10)))
	sts TCCR1B, r24
	sts TCNT1H, r1
	sts TCNT1L, r1
.endm

#endif // ATMEGA2560_AT2560_TIMERCONTEXT_INC
