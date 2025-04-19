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
 * @file initSys.c
 * @brief Implementation for init system
 *
 * @todo nothing
 */

#include <avr/io.h>

#include "sysCore/TaskMate_private_extern.h"
#include "sysCore/initSys.h"
#include "sysCore/autoInclude.h"

// initailize task memory
void threadCreate(void (*func)(void), uint8_t thread_id)
{
	modules.threads[thread_id].thread_id = thread_id;

	// RTC init
	modules.threads[thread_id].task_RTC = 0;

	// stack init
	modules.threads[thread_id].stack_pointer =
		&modules.threads[thread_id].stack[THREAD_STACK_SIZE - 1]; // get top of stack
	*(modules.threads[thread_id].stack_pointer--) = (uint16_t)func & 0xFF; // PCL;
	*(modules.threads[thread_id].stack_pointer--) = ((uint16_t)func>> 8) & 0xFF; // PCH
	*(modules.threads[thread_id].stack_pointer--) = 0x00; // PCHH always 0 if code size < 128k
	*(modules.threads[thread_id].stack_pointer--) = 0x00; // R0
	*(modules.threads[thread_id].stack_pointer--) = SREG;

	// Registers R1-R31
	for (int i = 1; i < 32; i++)
	{
		*(modules.threads[thread_id].stack_pointer--) = 0x00;
	}

	return;
}

void initServices(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [tag] service init
	threadCreate(lcd, 0);

	const char* task2_name = "lcd";
	modules.threads[2].thread_name = (uint8_t *)task2_name;
	modules.threads[2].setStatus = lcdSetStatus;
	modules.threads[2].getStatus = lcdGetStatus;
	(*modules.threads[2].setStatus)(5);

	threadCreate(scli, 1);

	const char* task3_name = "scli";
	modules.threads[3].thread_name = (uint8_t *)task3_name;
	modules.threads[3].setStatus = scliSetStatus;
	modules.threads[3].getStatus = scliGetStatus;
	(*modules.threads[3].setStatus)(5);
	// [/tag]
}

void initTasks(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [tag] task init
	threadCreate(task1, 2);

	const char* task0_name = "task1";
	modules.threads[0].thread_name = (uint8_t *)task0_name;
	modules.threads[0].setStatus = task1SetStatus;
	modules.threads[0].getStatus = task1GetStatus;
	(*modules.threads[0].setStatus)(3);

	threadCreate(task2, 3);

	const char* task1_name = "task2";
	modules.threads[1].thread_name = (uint8_t *)task1_name;
	modules.threads[1].setStatus = task2SetStatus;
	modules.threads[1].getStatus = task2GetStatus;
	(*modules.threads[1].setStatus)(3);
	// [/tag]
}

void initDrivers(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [tag] driver init
	const char* driver0_name = "timer1";
	modules.drivers[0]=(driver_item_t)
	{
		.driver_id = 0,
		.driver_name = (uint8_t *)driver0_name,
		.setStatus = timer1SetStatus,
		.getStatus = timer1GetStatus,
		.init = timer1Init,
		.start = timer1Start,
		.stop = timer1Stop
	};
	(*modules.drivers[0].setStatus)(3);

	const char* driver1_name = "timer3";
	modules.drivers[1]=(driver_item_t)
	{
		.driver_id = 1,
		.driver_name = (uint8_t *)driver1_name,
		.setStatus = timer3SetStatus,
		.getStatus = timer3GetStatus,
		.init = timer3Init,
		.start = timer3Start,
		.stop = timer3Stop
	};
	(*modules.drivers[1].setStatus)(3);

	const char* driver2_name = "i2c";
	modules.drivers[2]=(driver_item_t)
	{
		.driver_id = 2,
		.driver_name = (uint8_t *)driver2_name,
		.setStatus = i2cSetStatus,
		.getStatus = i2cGetStatus,
		.init = i2cInit,
		.start = i2cStart,
		.stop = i2cStop
	};
	(*modules.drivers[2].setStatus)(3);

	const char* driver3_name = "usart1";
	modules.drivers[3]=(driver_item_t)
	{
		.driver_id = 3,
		.driver_name = (uint8_t *)driver3_name,
		.setStatus = usart1SetStatus,
		.getStatus = usart1GetStatus,
		.init = usart1Init,
		.start = usart1Start,
		.stop = usart1Stop
	};
	(*modules.drivers[3].setStatus)(3);

	// [/tag]
}
