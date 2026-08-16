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

const int TIMER1_OVERFLOW_COUNT = 2000; // Interrupt every 1ms (1.10^-3 x 16.10^6 )/8 = 2000

static hal_timerSchedCallback_ptr_t sched_callback = NULL;

void hal_timerSchedSetCallback(hal_timerSchedCallback_ptr_t func_ptr) { sched_callback = func_ptr; }

void hal_timerSchedInit(void)
{
	// Set up timer1 interrupt for scheduler
	TCCR1B |= (uint8_t)(1u << WGM12); // CTC mode
	OCR1A = TIMER1_OVERFLOW_COUNT;
	TIMSK1 |= (uint8_t)(1u << OCIE1A);
}

#define TIMER_SCHED_START                              \
	"lds r24, %0\n\t"                                  \
	"ori r24, %1\n\t"                                  \
	"sts  %0, r24\n\t" : : "M"(_SFR_MEM_ADDR(TCCR1B)), \
						   "n"((uint8_t)(1u << CS11))  \
		: "r24"

void hal_timerSchedStart(void) { asm volatile(TIMER_SCHED_START); }

#define TIMER_SCHED_STOP                              \
	"lds r24, %0\n\t"                                 \
	"andi r24, %1\n\t"                                \
	"sts  %0, r24\n\t"                                \
	"sts %2,r1 \n\t"                                  \
	"sts %3,r1 \n\t" : : "M"(_SFR_MEM_ADDR(TCCR1B)),  \
						 "n"((uint8_t)~(1u << CS11)), \
						 "M"(_SFR_MEM_ADDR(TCNT1H)),  \
						 "M"(_SFR_MEM_ADDR(TCNT1L))   \
		: "r24"

void hal_timerSchedStop(void) { asm volatile(TIMER_SCHED_STOP); }

#define TM_SCHED_CALL_BACK                       \
	"in r24, 0x3d \n\t"                          \
	"in r25, 0x3e \n\t"                          \
	"lds r30, %0 \n\t"                           \
	"lds r31, %0+1 \n\t"                         \
	"movw r28,r24 \n\t"                          \
	"sbiw r30,0x00 \n\t"                         \
	"breq .+4 \n\t"                              \
	"eicall \n\t"                                \
	"out 0x3e, r25 \n\t"                         \
	"out 0x3d, r24 \n\t" : : "m"(sched_callback) \
		: "r24", "r25", "r30", "r31", "r28", "r29"

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	asm volatile(AVR8_CONTEXT_SAVE);
	asm volatile(TIMER_SCHED_STOP);

	asm volatile(TM_SCHED_CALL_BACK);

	asm volatile(TIMER_SCHED_START);
	asm volatile(AVR8_CONTEXT_RESTORE);
	asm volatile("reti \n\t");
}
