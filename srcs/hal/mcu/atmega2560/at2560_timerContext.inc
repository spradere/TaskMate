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

/* =============================================================================
 * Timer clock macros
 * ========================================================================== */

.macro TIMER_CONTEXT_START
	lds r24, 0x81
	ori r24, 0x02
	sts 0x81, r24
.endm

.macro TIMER_CONTEXT_STOP
	clr r1
	lds r24, 0x81
	andi r24, 0xF8
	sts 0x81, r24
	sts 0x85, r1
	sts 0x84, r1
.endm

#endif // ATMEGA2560_AT2560_TIMERCONTEXT_INC
