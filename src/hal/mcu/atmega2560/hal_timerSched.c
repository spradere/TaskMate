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
 * @file hal_timerSched.c
 * @brief hal part of scheduler
 *
 */

#include "hal/mcu/atmega2560/hal_timerSched.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <TaskMate.h>

const int TIMER1_OVERFLOW_COUNT = 2000; // Interrupt every 1ms (1.10^-3 x 16.10^6 )/8 = 2000

static hal_timerSchedCallback_t callback = NULL;
void hal_timerSchedSetCallback(hal_timerSchedCallback_t func_ptr) { callback = func_ptr; }

void hal_timerSchedInit(void)
{
	// Set up timer1 interrupt for scheduler
	TCCR1B = (uint8_t)(1u << CS11); // pre scaler = 8
	OCR1A = TIMER1_OVERFLOW_COUNT;
	TIMSK1 |= (uint8_t)(1u << OCIE1A);
}

void hal_timerSchedStart(void)
{
	TCNT1 = 0;
	// start by enabling source
	// WGM13 = 0 WGM12 = 1 WGM11 = 0 WGM10 = 0 -> CTC mode
	TCCR1A &= (uint8_t)~((1u << WGM11) | (1u << WGM10));
	TCCR1B |= (uint8_t)(1u << WGM12);
	TCCR1B &= (uint8_t)~(1u << WGM13);
}

void hal_timerSchedStop(void)
{
	// WGM13 = 0 WGM12 = 1 WGM11 = 0 WGM10 = 0 -> no source, timer stopped
	TCCR1A &= (uint8_t)~((1u << WGM11) | (1u << WGM10));
	TCCR1B &= (uint8_t)~((1u << WGM13) | (1u << WGM12));
}

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	// callback
	if( callback != NULL ) { callback(); }
}
