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
 * @todo Add read list files to get flags init and start at boot time
 */

#include <avr/io.h>

#include "sysCore/TaskMate_private_extern.h"
#include "sysCore/initSys.h"
#include "sysCore/autoIncludeTasks.h"
#include "sysCore/autoIncludeDrivers.h"

// create and initailize task memory
void taskCreate(void (*taskFunction)(void), uint8_t task_id)
{
	task_table[task_id].task_id = task_id;
	// task_table[task_id].task_name= ??

	// RTC init
	task_table[task_id].task_RTC = 0;

	// stack init
	task_table[task_id].stack_pointer =
		&task_table[task_id].stack[TASK_STACK_SIZE - 1]; // get to of stack
	*(task_table[task_id].stack_pointer--) = (uint16_t)taskFunction & 0xFF; // PCL;
	*(task_table[task_id].stack_pointer--) = ((uint16_t)taskFunction >> 8) & 0xFF; // PCH
	*(task_table[task_id].stack_pointer--) = 0x00; // PCHH always 0 if code size < 128k
	*(task_table[task_id].stack_pointer--) = 0x00; // R0
	*(task_table[task_id].stack_pointer--) = SREG;

	// Registers R1-R31
	for (int i = 1; i < 32; i++)
	{
		*(task_table[task_id].stack_pointer--) = 0x00;
	}

	return;
}

void initTasks(void)
{
	// do not edit code between tag : automatic generated code !
	// [tag] task init
	taskCreate(task1, 0);
	task_table[0].task_name = "task1";
	task_table[0].setStatus = task1SetStatus;
	task_table[0].getStatus = task1GetStatus;
	(*task_table[0].setStatus)(1);

	taskCreate(task2, 1);
	task_table[1].task_name = "task2";
	task_table[1].setStatus = task2SetStatus;
	task_table[1].getStatus = task2GetStatus;
	(*task_table[1].setStatus)(1);

	taskCreate(lcd, 2);
	task_table[2].task_name = "lcd";
	task_table[2].setStatus = lcdSetStatus;
	task_table[2].getStatus = lcdGetStatus;
	(*task_table[2].setStatus)(1);

	taskCreate(scli, 3);
	task_table[3].task_name = "scli";
	task_table[3].setStatus = scliSetStatus;
	task_table[3].getStatus = scliGetStatus;
	(*task_table[3].setStatus)(1);

	// [/tag]
}

void initDrivers(void)
{
	// do not edit code between tag : automatic generated code !
	// [tag] driver init
	driver_table[0]=(driver_table_t)
	{
		.driver_id = 0,
		.driver_name = "timer1",
		.setStatus = timer1SetStatus,
		.getStatus = timer1GetStatus,
		.init = timer1Init,
		.start = timer1Start,
		.stop = timer1Stop
	};
	(*driver_table[0].setStatus)(3);

	driver_table[1]=(driver_table_t)
	{
		.driver_id = 1,
		.driver_name = "timer3",
		.setStatus = timer3SetStatus,
		.getStatus = timer3GetStatus,
		.init = timer3Init,
		.start = timer3Start,
		.stop = timer3Stop
	};
	(*driver_table[1].setStatus)(3);

	driver_table[2]=(driver_table_t)
	{
		.driver_id = 2,
		.driver_name = "i2c",
		.setStatus = i2cSetStatus,
		.getStatus = i2cGetStatus,
		.init = i2cInit,
		.start = i2cStart,
		.stop = i2cStop
	};
	(*driver_table[2].setStatus)(3);

	driver_table[3]=(driver_table_t)
	{
		.driver_id = 3,
		.driver_name = "usart1",
		.setStatus = usart1SetStatus,
		.getStatus = usart1GetStatus,
		.init = usart1Init,
		.start = usart1Start,
		.stop = usart1Stop
	};
	(*driver_table[3].setStatus)(3);

	// [/tag]

	// driver flag init -> todo : change by reading file list and write driver satus.
	uint8_t i;
	for (i = 0; i < DRIVER_COUNT; i++)
	{
		(*driver_table[i].setStatus)((1 << DRIVER_INIT_AT_BOOT) | (1 << DRIVER_START_AT_BOOT));
	}
}
