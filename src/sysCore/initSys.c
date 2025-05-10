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
 * @file initSys.c
 * @brief Implementation for init system
 *
 * @todo nothing
 */

#include <avr/io.h>

#include "sysCore/TaskMate_private_extern.h"
#include "sysCore/initSys.h"
#include "sysCore/autoInclude.h"
#include "sysCore/run_level_define.h"

#define AVR_REGISTER_COUNT 32 // from R0 to R31

// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

// initailize thread memory
static void threadCreate(void (*func)(void), uint8_t num)
{
	// modules.threads[num].num = num;
	modules.threads[num].time_counter = 0;

	// stack init
	modules.threads[num].stack_pointer =
		&modules.threads[num].stack[THREAD_STACK_SIZE - 1]; // get top of stack
	*(modules.threads[num].stack_pointer--) = (uint16_t)func & 0xFF; // PCL;
	*(modules.threads[num].stack_pointer--) = ((uint16_t)func >> 8) & 0xFF; // PCH
	*(modules.threads[num].stack_pointer--) = 0x00; // PCHH always 0 if code size < 128k
	*(modules.threads[num].stack_pointer--) = 0x00; // R0
	*(modules.threads[num].stack_pointer--) = SREG;

	// Registers R1-R31
	for( int i = 1; i < AVR_REGISTER_COUNT; i++ ) { *(modules.threads[num].stack_pointer--) = 0x00; }
}

void initThreads(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [tag] threads init
	threadCreate(lcd, 0);

	const char *thread0_name = "lcd";
	modules.threads[0].id = 2000;
	modules.threads[0].name = (uint8_t *)thread0_name;
	modules.threads[0].setStatus = lcdSetStatus;
	modules.threads[0].getStatus = lcdGetStatus;
	(*modules.threads[0].setStatus)(19); // set run level | thread type

	threadCreate(scli, 1);

	const char *thread1_name = "scli";
	modules.threads[1].id = 2001;
	modules.threads[1].name = (uint8_t *)thread1_name;
	modules.threads[1].setStatus = scliSetStatus;
	modules.threads[1].getStatus = scliGetStatus;
	(*modules.threads[1].setStatus)(17); // set run level | thread type

	threadCreate(task1, 2);

	const char *thread2_name = "task1";
	modules.threads[2].id = 3000;
	modules.threads[2].name = (uint8_t *)thread2_name;
	modules.threads[2].setStatus = task1SetStatus;
	modules.threads[2].getStatus = task1GetStatus;
	(*modules.threads[2].setStatus)(12); // set run level | thread type

	threadCreate(task2, 3);

	const char *thread3_name = "task2";
	modules.threads[3].id = 3001;
	modules.threads[3].name = (uint8_t *)thread3_name;
	modules.threads[3].setStatus = task2SetStatus;
	modules.threads[3].getStatus = task2GetStatus;
	(*modules.threads[3].setStatus)(12); // set run level | thread type

	// [/tag]
}

void initDrivers(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [tag] drivers init
	const char *driver0_name = "timer1";
	modules.drivers[0] = (driver_item_t){.id = 1000,
										 .name = (uint8_t *)driver0_name,
										 .setStatus = timer1SetStatus,
										 .getStatus = timer1GetStatus,
										 .init = timer1Init,
										 .start = timer1Start,
										 .stop = timer1Stop};
	(*modules.drivers[0].setStatus)(1);

	const char *driver1_name = "timer3";
	modules.drivers[1] = (driver_item_t){.id = 1001,
										 .name = (uint8_t *)driver1_name,
										 .setStatus = timer3SetStatus,
										 .getStatus = timer3GetStatus,
										 .init = timer3Init,
										 .start = timer3Start,
										 .stop = timer3Stop};
	(*modules.drivers[1].setStatus)(1);

	const char *driver2_name = "i2c";
	modules.drivers[2] = (driver_item_t){.id = 1002,
										 .name = (uint8_t *)driver2_name,
										 .setStatus = i2cSetStatus,
										 .getStatus = i2cGetStatus,
										 .init = i2cInit,
										 .start = i2cStart,
										 .stop = i2cStop};
	(*modules.drivers[2].setStatus)(2);

	const char *driver3_name = "usart1";
	modules.drivers[3] = (driver_item_t){.id = 1003,
										 .name = (uint8_t *)driver3_name,
										 .setStatus = usart1SetStatus,
										 .getStatus = usart1GetStatus,
										 .init = usart1Init,
										 .start = usart1Start,
										 .stop = usart1Stop};
	(*modules.drivers[3].setStatus)(1);

	// [/tag]
}

void initRunLevels(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [tag] run levels
	to_run =
		(run_levels_t){.level0 = {0},
					   .level1 = {4, 1000, 1001, 1003, 2001},
					   .level2 = {1, 1002},
					   .level3 = {1, 2000},
					   .level4 = {2, 3000, 3001},
					   .levels = {to_run.level0, to_run.level1, to_run.level2, to_run.level3, to_run.level4}};
	to_run.current = RUN_CORE;
	to_run.next = RUN_CORE;
	// [/tag]
}
// NOLINTEND
