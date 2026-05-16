/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file timerSched.c
 * @brief atmega2560 hal_timerSched_impl implementation.
 *
 */

#include "hal/mcu/atmega2560/timerSched.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "hal/arch/avr8/context.h"
#include "hal/arch/avr8/int.h"
#include "hal/arch/avr8/stack.h"
#include "interfaces/define.h"
// #include "tm_libc/tm_syslog.h"

const int TIMER1_OVERFLOW_COUNT = 2000; // Interrupt every 1ms (1.10^-3 x 16.10^6 )/8 = 2000
// const int TIMER1_OVERFLOW_COUNT = 15625; // Interrupt every 1s (1 x 16.10^6 )/1024 = 15625

static hal_timerSchedCallback_ptr_t sched_callback = NULL;

void hal_timerSchedSetCallback(hal_timerSchedCallback_ptr_t func_ptr)
{
	uintptr_t p = (uintptr_t)func_ptr;
	// tm_syslog(TM_STR("[timer sched] callback = 0x%04x\n"), (p << 1));
	sched_callback = func_ptr;
}

void hal_timerSchedInit(void)
{
	// Set up timer1 interrupt for scheduler
	TCCR1B |= (uint8_t)(1u << WGM12); // CTC mode
	OCR1A = TIMER1_OVERFLOW_COUNT;
	TIMSK1 |= (uint8_t)(1u << OCIE1A);
}

void hal_timerSchedStart(void)
{
	TCNT1 = 0;
	TCCR1B = (uint8_t)(1u << CS11); // pre scaler = 8
	// TCCR1B = (uint8_t)((1u << CS12) | (1u << CS10)); // pre scaler = 1024
}

void hal_timerSchedStop(void)
{
	TCCR1B &= (uint8_t)~(1u << CS11); // pre scaler = no source
}

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	hal_contextSave();
	hal_timerSchedStop();

	hal_stack_word_t *sp_current;
	hal_stack_word_t *sp_next;

	sp_current = hal_getStackPointer();

	sp_next = sp_current;

	// scheduler callback
	if( sched_callback != NULL ) { sp_next = sched_callback(sp_current); }

	hal_timerSchedStart();
	hal_setStackPointer(sp_next);
	hal_contextRestore();
	hal_setGlobalInterupt();
	hal_returnFromInterupt();
}
