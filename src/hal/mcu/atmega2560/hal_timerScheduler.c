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

/**
 * @file hal_timerScheduler.c
 * @brief hal part of schuduler
 *
 * @todo nothing
 */

#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>

#include "sysCore/TaskMate_private_extern.h"
#include "sysCore/scheduler.h"

const int TIMER1_OVERFLOW_COUNT = 2000; // Interrupt every 1ms (1.10^-3 x 16.10^6 )/8 = 2000

void hal_timerSchedulerInit(void)
{
	// Set up timer1 interrupt for scheduler
	TCCR1A = 0; // WGM11 = 0 WGM10 = 0
	TCCR1B = (1 << CS11); // prescaler = 8
	OCR1A = TIMER1_OVERFLOW_COUNT;
	TIMSK1 |= (1 << OCIE1A);
}

void hal_timerSchedulerStart(void)
{
	TCNT1 = 0;
	// start by enabling source
	//WGM13 = 0 WGM12 = 1 WGM11 = 0 WGM10 = 0 -> CTC mode
	TCCR1A &= ~( (1 << WGM11) | (1 << WGM10) );
	TCCR1B |= (1 << WGM12);
	TCCR1B &= ~(1 << WGM13);

}

void hal_timerSchedulerStop(void)
{
	// WGM13 = 0 WGM12 = 1 WGM11 = 0 WGM10 = 0 -> no source, timer stopped
	TCCR1A &= ~( (1 << WGM11) | (1 << WGM10) );
	TCCR1B &= ~( (1 << WGM13) | (1 << WGM12) );
}

void hal_timerSchedulerLoad(void)
{
	// used for cooperative yield hand to scheduler
	sysCallSetFlag(FLAG_COOP);
	ATOMIC_BLOCK(ATOMIC_FORCEON) { TCNT1 = TIMER1_OVERFLOW_COUNT - 1; }
	while( sysCallGetFlag(FLAG_COOP) == 1 );
}

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	// save current thread context
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		hal_contextSave();
		modules.threads[modules.thread_current].stack_pointer = (stack_word_t *)hal_getStackPointer();
	}

	scheduler();

	// restore next thread context
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		hal_setStackPointer((uintptr_t)modules.threads[modules.thread_current].stack_pointer);
		hal_contextRestore();
		hal_returnFromInterupt();
	}
}
