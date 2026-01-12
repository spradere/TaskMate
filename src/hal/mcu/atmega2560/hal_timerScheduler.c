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
 */

#include "hal/mcu/atmega2560/hal_timerScheduler.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "hal/auto_hal_system.h"
#include "sysCall/sysCall.h"
#include "sysCore/modules.h"
#include "sysCore/tm_scheduler.h"

const int TIMER1_OVERFLOW_COUNT = 2000; // Interrupt every 1ms (1.10^-3 x 16.10^6 )/8 = 2000

void hal_timerSchedulerInit(void)
{
	// Set up timer1 interrupt for scheduler
	TCCR1A = 0; // WGM11 = 0 WGM10 = 0
	TCCR1B = (uint8_t)(1u << CS11); // prescaler = 8
	OCR1A = TIMER1_OVERFLOW_COUNT;
	TIMSK1 |= (uint8_t)(1u << OCIE1A);
}

void hal_timerSchedulerStart(void)
{
	TCNT1 = 0;
	// start by enabling source
	// WGM13 = 0 WGM12 = 1 WGM11 = 0 WGM10 = 0 -> CTC mode
	TCCR1A &= (uint8_t)~((1u << WGM11) | (1u << WGM10));
	TCCR1B |= (uint8_t)(1u << WGM12);
	TCCR1B &= (uint8_t)~(1u << WGM13);
}

void hal_timerSchedulerStop(void)
{
	// WGM13 = 0 WGM12 = 1 WGM11 = 0 WGM10 = 0 -> no source, timer stopped
	TCCR1A &= (uint8_t)~((1u << WGM11) | (1u << WGM10));
	TCCR1B &= (uint8_t)~((1u << WGM13) | (1u << WGM12));
}

void hal_timerSchedulerLoad(void)
{
	// used for cooperative yield hand to scheduler
	sc_flagSet(FLAG_COOP);
	ATOMIC_BLOCK(ATOMIC_FORCEON) { TCNT1 = TIMER1_OVERFLOW_COUNT - 1; }
	while( sc_flagGet(FLAG_COOP) == 1 );
}

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	mod_thread_item_t *mod;

	// save current thread context
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		hal_contextSave();
		mod = mod_threadGetPointer(mod_threadGetCurrent());
		mod->stack_pointer = (hal_stack_word_t *)hal_getStackPointer();
	}

	tm_scheduler();

	// restore next thread context
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		mod = mod_threadGetPointer(mod_threadGetCurrent());
		hal_setStackPointer((uintptr_t)mod->stack_pointer);
		hal_contextRestore();
		hal_returnFromInterupt();
	}
}
