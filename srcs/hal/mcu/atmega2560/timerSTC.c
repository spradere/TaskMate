/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file timerSTC.c
 * @brief timer stc implementation.
 *
 */

#include "timerSTC.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "interfaces/define.h"
#include "interfaces/macros.h"

const uint16_t hal_timerSTC_OVERFLOW_COUNT =
	625; // Interrupt every 10ms (10.10^-3 x 16.10^6 )/256 = 625

static hal_timerSTCCallback_t stc_callback = NULL;
void hal_timerSTCSetCallback(hal_timerSTCCallback_t func_ptr) { stc_callback = func_ptr; }

void hal_timerSTCInit(void)
{

	// Set up timer3 for RTC
	TM_WRITEBIT(TCCR3B, WGM32, CS32); // CTC mode, prescaler 256
	OCR3A = hal_timerSTC_OVERFLOW_COUNT;
}

void hal_timerSTCStart(void)
{
	// start by enabling interrupt
	TM_SETBIT(TIMSK3, OCIE3A);
}

void hal_timerSTCStop(void)
{
	// stop by disabling interrupt
	TM_CLEARBIT(TIMSK3, OCIE3A);
}

ISR(TIMER3_COMPA_vect)
{
	// Software time counter callback
	if( stc_callback != NULL ) { stc_callback(); }
}
