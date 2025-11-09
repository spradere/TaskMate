/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

#include <avr/io.h>
#include <util/atomic.h>
#include "sysCore/TaskMate_public.h"
#include "arch/avr8/timer1.h"

const int TIMER1_OVERFLOW_COUNT = 2000; // Interrupt every 1ms (1.10^-3 x 16.10^6 )/8 = 2000

void timer1Init(void)
{
	// Set up timer1 interrupt for scheduler
	TCCR1B |= (1 << WGM12) | (1 << CS11); // CTC mode, prescaler 8
	OCR1A = TIMER1_OVERFLOW_COUNT;
}

void timer1Start(void)
{
	// start by enable INT
	TIMSK1 |= (1 << OCIE1A);
}

void timer1Stop(void)
{
	// nothing to do, will stop all system.
}

void timer1LoadOverflow(void)
{
	// used for cooperative yield hand to scheduler
	sysCallSetFlag(FLAG_COOP);
	ATOMIC_BLOCK(ATOMIC_FORCEON) { TCNT1 = TIMER1_OVERFLOW_COUNT - 1; }
	while( sysCallGetFlag(FLAG_COOP) == 1 );
}
