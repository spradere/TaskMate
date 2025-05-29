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
void threadCreate(void (*func)(void), uint8_t num)
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
	threadCreate(scli, 0);

	const char *thread0_name = "scli";
	modules.threads[0].name = (uint8_t *)thread0_name;
	modules.threads[0].status = 17; // set run level | thread type
	modules.threads[0].main = scli;
	threadCreate(msg, 1);

	const char *thread1_name = "msg";
	modules.threads[1].name = (uint8_t *)thread1_name;
	modules.threads[1].status = 19; // set run level | thread type
	modules.threads[1].main = msg;
	threadCreate(task1, 2);

	const char *thread2_name = "task1";
	modules.threads[2].name = (uint8_t *)thread2_name;
	modules.threads[2].status = 12; // set run level | thread type
	modules.threads[0].main = task1;
	threadCreate(task2, 3);

	const char *thread3_name = "task2";
	modules.threads[3].name = (uint8_t *)thread3_name;
	modules.threads[3].status = 12; // set run level | thread type
	modules.threads[1].main = task2;
	// [/tag]
}

void initDrivers(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [tag] drivers init
	const char *driver0_name = "timer1";
	modules.drivers[0]=(driver_item_t)
	{
		.name = (uint8_t *)driver0_name,
		.status = 1,
		.init = timer1Init,
		.start = timer1Start,
		.stop = timer1Stop
	};
	const char *driver1_name = "timer3";
	modules.drivers[1]=(driver_item_t)
	{
		.name = (uint8_t *)driver1_name,
		.status = 1,
		.init = timer3Init,
		.start = timer3Start,
		.stop = timer3Stop
	};
	const char *driver2_name = "i2c";
	modules.drivers[2]=(driver_item_t)
	{
		.name = (uint8_t *)driver2_name,
		.status = 1,
		.init = i2cInit,
		.start = i2cStart,
		.stop = i2cStop
	};
	const char *driver3_name = "usart1";
	modules.drivers[3]=(driver_item_t)
	{
		.name = (uint8_t *)driver3_name,
		.status = 1,
		.init = usart1Init,
		.start = usart1Start,
		.stop = usart1Stop
	};
	const char *driver4_name = "lcdAMC2004";
	modules.drivers[4]=(driver_item_t)
	{
		.name = (uint8_t *)driver4_name,
		.status = 2,
		.init = lcdAMC2004Init,
		.start = lcdAMC2004Start,
		.stop = lcdAMC2004Stop
	};
	// [/tag]
}

void initRunLevels(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [tag] run levels
	to_run = (run_levels_t){
		.level0 = {0},
		.level1 = {1,0},
		.level2 = {1,0},
		.level3 = {2,0,1},
		.level4 = {4,0,1,2,3},
		.levels = {to_run.level0, to_run.level1, to_run.level2, to_run.level3, to_run.level4}
	};
	to_run.current=RUN_CORE;
	to_run.next=RUN_CORE;
	// [/tag]
}
// NOLINTEND
