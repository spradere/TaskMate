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
	task_table[thread_id].task_id = thread_id;

	// RTC init
	task_table[thread_id].task_RTC = 0;

	// stack init
	task_table[thread_id].stack_pointer =
		&task_table[thread_id].stack[TASK_STACK_SIZE - 1]; // get top of stack
	*(task_table[thread_id].stack_pointer--) = (uint16_t)func & 0xFF; // PCL;
	*(task_table[thread_id].stack_pointer--) = ((uint16_t)func>> 8) & 0xFF; // PCH
	*(task_table[thread_id].stack_pointer--) = 0x00; // PCHH always 0 if code size < 128k
	*(task_table[thread_id].stack_pointer--) = 0x00; // R0
	*(task_table[thread_id].stack_pointer--) = SREG;

	// Registers R1-R31
	for (int i = 1; i < 32; i++)
	{
		*(task_table[thread_id].stack_pointer--) = 0x00;
	}

	return;
}

void initServices(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [tag] service init
	threadCreate(lcd, 0);

	const char* task2_name = "lcd";
	task_table[2].task_name = (uint8_t *)task2_name;
	task_table[2].setStatus = lcdSetStatus;
	task_table[2].getStatus = lcdGetStatus;
	(*task_table[2].setStatus)(5);

	threadCreate(scli, 1);

	const char* task3_name = "scli";
	task_table[3].task_name = (uint8_t *)task3_name;
	task_table[3].setStatus = scliSetStatus;
	task_table[3].getStatus = scliGetStatus;
	(*task_table[3].setStatus)(5);
	// [/tag]
}

void initTasks(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [tag] task init
	threadCreate(task1, 2);

	const char* task0_name = "task1";
	task_table[0].task_name = (uint8_t *)task0_name;
	task_table[0].setStatus = task1SetStatus;
	task_table[0].getStatus = task1GetStatus;
	(*task_table[0].setStatus)(3);

	threadCreate(task2, 3);

	const char* task1_name = "task2";
	task_table[1].task_name = (uint8_t *)task1_name;
	task_table[1].setStatus = task2SetStatus;
	task_table[1].getStatus = task2GetStatus;
	(*task_table[1].setStatus)(3);
	// [/tag]
}

void initDrivers(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [tag] driver init
	const char* driver0_name = "timer1";
	driver_table[0]=(driver_item_t)
	{
		.driver_id = 0,
		.driver_name = (uint8_t *)driver0_name,
		.setStatus = timer1SetStatus,
		.getStatus = timer1GetStatus,
		.init = timer1Init,
		.start = timer1Start,
		.stop = timer1Stop
	};
	(*driver_table[0].setStatus)(3);

	const char* driver1_name = "timer3";
	driver_table[1]=(driver_item_t)
	{
		.driver_id = 1,
		.driver_name = (uint8_t *)driver1_name,
		.setStatus = timer3SetStatus,
		.getStatus = timer3GetStatus,
		.init = timer3Init,
		.start = timer3Start,
		.stop = timer3Stop
	};
	(*driver_table[1].setStatus)(3);

	const char* driver2_name = "i2c";
	driver_table[2]=(driver_item_t)
	{
		.driver_id = 2,
		.driver_name = (uint8_t *)driver2_name,
		.setStatus = i2cSetStatus,
		.getStatus = i2cGetStatus,
		.init = i2cInit,
		.start = i2cStart,
		.stop = i2cStop
	};
	(*driver_table[2].setStatus)(3);

	const char* driver3_name = "usart1";
	driver_table[3]=(driver_item_t)
	{
		.driver_id = 3,
		.driver_name = (uint8_t *)driver3_name,
		.setStatus = usart1SetStatus,
		.getStatus = usart1GetStatus,
		.init = usart1Init,
		.start = usart1Start,
		.stop = usart1Stop
	};
	(*driver_table[3].setStatus)(3);

	// [/tag]
}
