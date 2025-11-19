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

// test inline
// todo add this to autoInclude.h
#include "hal/arch/avr8/hal_stack.h"
#include "hal/arch/avr8/hal_context.h"

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
////////////////////////////////////////////////////////////////////////////////
// new HAL implementation

	hal_archInit();
	hal_mcuInit();
	hal_boardInit();

////////////////////////////////////////////////////////////////////////////////
// old implementation

	initDrivers();
	initThreads();
	runLevelInit();

	// start driver
	for( uint8_t i = 0; i < DRIVERS_COUNT; i++ )
	{
		(*modules.drivers[i].init)();
		(*modules.drivers[i].start)();
	}

	// jump to current thread for first call and start system by enabling INT
	modules.thread_current = 0;

	hal_setStackPointer((uintptr_t)modules.threads[modules.thread_current].stack_pointer);

	hal_contextRestore();
	hal_setGlobalInterupt();
	hal_returnFromInterupt();

	return 0; // You should never get here
}

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	// save current thread context
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		hal_contextSave();
		modules.threads[modules.thread_current].stack_pointer = (stack_word_t *)hal_getStackPointer();
	}

	// enable global INT to let run timer3 RTC and usart1 sCLI
	hal_setGlobalInterupt();

	// stop timer1 prevent preemption of the scheduler itself -> panic
	// prevent scheduler eat thread time slice
	//**timer1Stop();
	hal_timerSchedulerStop();


	// todo -> add stack overflow test

	// todo -> add system wide error handler

	// switch context
	if( ++modules.thread_current == THREADS_COUNT ) { modules.thread_current = 0; }

	// I'm alive blink in board led
	static uint8_t alive_cnt = 0;
	if( ++alive_cnt > 250 )
	{
		hal_inBoardLed(HAL_IN_BOARD_LED_TOGGLE);
		alive_cnt = 0;
	}

	// cooperative handling
	sysCallClearFlag(FLAG_COOP);

	//**timer1Start();
	hal_timerSchedulerStart();

	// restore next thread context
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		hal_setStackPointer((uintptr_t)modules.threads[modules.thread_current].stack_pointer);
		hal_contextRestore();
		hal_returnFromInterupt();
	}
}
