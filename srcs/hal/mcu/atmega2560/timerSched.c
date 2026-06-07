/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file timerSched.c
 * @brief timer sched implementation.
 *
 */

#include "timerSched.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "hal/arch/avr8/context.h"
#include "hal/arch/avr8/interrupt.h"
#include "hal/arch/avr8/stack.h"
#include "interfaces/define.h"
// #include "tm_libc/tm_syslog.h"

const int TIMER1_OVERFLOW_COUNT = 2000; // Interrupt every 1ms (1.10^-3 x 16.10^6 )/8 = 2000
// const int TIMER1_OVERFLOW_COUNT = 15625; // Interrupt every 1s (1 x 16.10^6 )/1024 = 15625

static hal_timerSchedCallback_ptr_t sched_callback = NULL;

void hal_timerSchedSetCallback(hal_timerSchedCallback_ptr_t func_ptr)
{
	//uintptr_t p = (uintptr_t)func_ptr;
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

#define ATMEGA2560_TIMERSCHED_START TCCR1B = (uint8_t)(1u << CS11)
void hal_timerSchedStart(void)
{
	ATMEGA2560_TIMERSCHED_START; 
	// TCCR1B = (uint8_t)((1u << CS12) | (1u << CS10)); // pre scaler = 1024
}

#define ATMEGA2560_TIMERSCHED_STOP TCCR1B &= (uint8_t)~(1u << CS11); TCNT1 = 0
void hal_timerSchedStop(void)
{
	ATMEGA2560_TIMERSCHED_STOP; 
	/*asm volatile( 
		"lds r24, %0\n\t"
		"andi r24, %1\n\t"
		"sts  %0, r24\n\t"
		"sts %2,r1 \n\t"
		"sts %3,r1 \n\t"
		:
		: "M" (_SFR_MEM_ADDR(TCCR1B)), "n" ((uint8_t)~(1u << CS11)),
		"M" (_SFR_MEM_ADDR(TCNT1L)), "M" (_SFR_MEM_ADDR(TCNT1H))
		: "r24"
	);*/
}

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	asm volatile(AVR8_CONTEXT_SAVE);
	//hal_contextSave();
	
	//hal_timerSchedStop();
	ATMEGA2560_TIMERSCHED_STOP;

	hal_stack_word_t *sp_current = (hal_stack_word_t*)SP;
	hal_stack_word_t *sp_next = (hal_stack_word_t*)SP;

	//sp_current = hal_getStackPointer();
	//sp_next = sp_current;

	// scheduler callback
	if( sched_callback != NULL ) { sp_next = sched_callback(sp_current); }

	//hal_timerSchedStart();
	ATMEGA2560_TIMERSCHED_START;
	
	//hal_setStackPointer(sp_next);
	SP = (uintptr_t)sp_next;
	
	asm volatile(AVR8_CONTEXT_RESTORE);
	//hal_contextRestore();
	
	//hal_setGlobalInterupt();
	asm volatile("reti \n\t");
	//hal_returnFromInterupt();
}
