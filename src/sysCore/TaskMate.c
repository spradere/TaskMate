/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file TaskMate.c
 * @brief Implements TaskMate preemptive scheduler & RTC.
 *
 * This file contains :
 * - system, drivers and task initialisation
 *
 * @todo Atomic for SP read/write, sequence for startup.
 * Now or later you will have to write a very light wight libc,
 * designed for embedded system with MCU. Not POSIX compliant.
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "sysCore/TaskMate_define.h"
#include "sysCore/initSys.h"

#include "sysCore/autoInclude.h"
#include "sysCore/autoAlloc.h"


int main(void)
{
	initDrivers();
	initServices();
	initTasks();

	// Set output for in board led 13
	LED_DDR |= (1 << LED_PIN);

	uint8_t i;

	// init driver if flag on
	for (i = 0; i < DRIVER_COUNT; i++)
	{
		if (((*driver_table[i].getStatus)() & (1 << DRIVER_INIT_AT_BOOT)) != 0)
		{
			(*driver_table[i].init)();
		}
	}

	// start driver if flag on
		for (i = 0; i < DRIVER_COUNT; i++)
	{
		if (((*driver_table[i].getStatus)() & (1 << DRIVER_START_AT_BOOT)) != 0)
		{
			(*driver_table[i].start)();
		}
	}

	// jump to current task for first call and start system by enabling INT
	task_current = 0;
	SP = (uint16_t)task_table[task_current].stack_pointer;
	asm volatile(POP_ALL_REGS "sei \n\t"
							  "ret \n\t");

	return 0;
}

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	// enable global INT to catch RTC INT without delay
	sei();

	// Save current task context
	asm volatile(PUSH_ALL_REGS);
	task_table[task_current].stack_pointer = (uint8_t *)SP;

	// todo -> add stack overflow test

	// todo -> add system wide error handler

	// switch context
	if (++task_current == TASK_COUNT)
	{
		task_current = 0;
	}

	// I'm alive blink in board led 13
	static uint8_t alive_cnt = 0;
	if (++alive_cnt > 250)
	{
		LED_PORT ^= (1 << LED_PIN);
		alive_cnt = 0;
	}

	// Restore next task context
	SP = (uint16_t)task_table[task_current].stack_pointer;
	asm volatile(POP_ALL_REGS "reti \n\t");
}
