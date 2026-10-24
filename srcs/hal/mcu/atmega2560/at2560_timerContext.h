/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file at2560_timerContext.h
 * @brief ATmega2560 context-switch timer register definitions.
 *
 */

#ifndef ATMEGA2560_AT2560_TIMERCONTEXT_H
#define ATMEGA2560_AT2560_TIMERCONTEXT_H

/* ============================================================================
 * Includes
 * ========================================================================== */
#ifdef __ASSEMBER__
	#include <avr/io.h>
#endif

/* ============================================================================
 * Public definitions
 * ========================================================================== */

#define AT2560TIMERCONTEXT_BIT_CS10 0
#define AT2560TIMERCONTEXT_BIT_CS11 1
#define AT2560TIMERCONTEXT_BIT_CS12 2
#define AT2560TIMERCONTEXT_ADDR_TCCR1B 0x80
#define AT2560TIMERCONTEXT_ADDR_TCNT1H 0x85
#define AT2560TIMERCONTEXT_ADDR_TCNT1L 0x84

#ifdef __ASSEMBER__
	_Static_assert(CS10 == AT2560TIMERCONTEXT_BIT_CS10, "Unexpected CS10 position");
	_Static_assert(CS11 == AT2560TIMERCONTEXT_BIT_CS11, "Unexpected CS11 position");
	_Static_assert(CS12 == AT2560TIMERCONTEXT_BIT_CS12, "Unexpected CS12 position");
	_Static_assert(_SFR_MEM_ADDR(TCCR1B) == AT2560TIMERCONTEXT_ADDR_TCCR1B,"Unexpected TCCR1B address");
	_Static_assert(_SFR_MEM_ADDR(TCNT1H) == AT2560TIMERCONTEXT_ADDR_TCNT1H,"Unexpected TCNT1H address");
	_Static_assert(_SFR_MEM_ADDR(TCNT1L) == AT2560TIMERCONTEXT_ADDR_TCNT1L,"Unexpected TCNT1L address");
#endif

#endif // ATMEGA2560_AT2560_TIMERCONTEXT_H
