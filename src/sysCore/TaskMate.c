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
 * @file TaskMate.c
 * @brief Implements TaskMate preemptive scheduler & RTC.
 *
 * This file contains :
 * - system, drivers and thread initialisation
 *
 * @todo finish run level : add init service for startup
 *
 * Now or later I will have to write a very lightweight libc,
 * designed for embedded systems with MCU. Not POSIX compliant.
 *
 *
 * sysCallPreemptProtected(timeout, driver);
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "sysCore/TaskMate_define.h"
#include "sysCore/sysCall.h"
#include "sysCore/initSys.h"
#include "sysCore/modules_items.h"
#include "sysCore/autoInclude.h"
#include "sysCore/autoAlloc.h"
#include "sysCore/runLevel.h"

// system core wide variables
modules_t modules;

int main(void)
{
	initDrivers();
	initThreads();
	runLevelInit();

	// Set output for in board led 13
	LED_DDR |= (1 << LED_PIN);

	// start driver
	for( uint8_t i = 0; i < DRIVERS_COUNT; i++ )
	{
		(*modules.drivers[i].init)();
		(*modules.drivers[i].start)();
	}

	// jump to current thread for first call and start system by enabling INT
	modules.thread_current = 0;
	SP = (uint16_t)modules.threads[modules.thread_current].stack_pointer;
	asm volatile(POP_ALL_REGS "sei \n\t"
							  "ret \n\t");

	return 0; // You should never get here
}

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	// save current thread context
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		asm volatile(PUSH_ALL_REGS);
		modules.threads[modules.thread_current].stack_pointer = (uint8_t *)SP;
	}

	// enable global INT to let run timer3 RTC and usart1 sCLI
	sei();

// stop timer1 prevent preemption of the scheduler itself -> panic
// prevent scheduler eat thread time slice
#define TIMER1_CS_MASK 0x07 // 3 lsb bits of TCCR are CS

	uint8_t timer1_CS = TCCR1B;
	timer1_CS &= TIMER1_CS_MASK;
	TCCR1B &= ~TIMER1_CS_MASK; // CS12 CS11 CS10 = 0 0 0 no source, timer stopped

	// todo -> add stack overflow test

	// todo -> add system wide error handler

	// switch context
	if( ++modules.thread_current == THREADS_COUNT ) { modules.thread_current = 0; }

	// I'm alive blink in board led
	static uint8_t alive_cnt = 0;
	if( ++alive_cnt > 250 )
	{
		LED_PORT ^= (1 << LED_PIN);
		alive_cnt = 0;
	}

	// cooperative handling
	sysCallClearFlag(FLAG_COOP);

	// reset / resart timer1
	TCNT1 = 0;
	TCCR1B |= timer1_CS;

	// restore next thread context
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		SP = (uint16_t)modules.threads[modules.thread_current].stack_pointer;
		asm volatile(POP_ALL_REGS "reti \n\t");
	}
}
