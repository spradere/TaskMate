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

#include "hal/mcu/atmega2560/hal_timerSched_impl.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "hal/public/hal_context.h"
#include "hal/public/hal_stack.h"

#include "tm_libc/tm_syslog.h"


//const int TIMER1_OVERFLOW_COUNT = 2000; // Interrupt every 1ms (1.10^-3 x 16.10^6 )/8 = 2000
const int TIMER1_OVERFLOW_COUNT = 15625; // Interrupt every 1ms (1 x 16.10^6 )/1024 = 15625


static hal_timerSchedCallback_t sched_callback = NULL;
void hal_timerSchedSetCallback(hal_timerSchedCallback_t func_ptr)
{
	uintptr_t p = (uintptr_t)func_ptr;
	tm_syslog(TM_STR("[timer sched] callback = 0x%04x\n"), (p<<1));
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
	//TCCR1B = (uint8_t)(1u << CS11); // pre scaler = 8
	TCCR1B = (uint8_t)((1u << CS12) | (1u << CS10)); // pre scaler = 1024

}

void hal_timerSchedStop(void)
{
	TCCR1B &= (uint8_t)~(1u << CS11); // pre scaler = no source
}

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	// scheduler callback
	if( sched_callback != NULL ) { sched_callback(); }

	hal_returnFromInterupt();
}
