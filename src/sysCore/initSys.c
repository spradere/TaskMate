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
#include "hal/hal_api.h"

// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

void initThreads(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [autoCode_tag] threads init

	hal_threadContextInit(scli, &modules.threads[0].stack_pointer, &modules.threads[0].stack[THREAD_STACK_SIZE -1 ]);
	modules.threads[0].real_time_counter = 0;
	const char *thread0_name = "scli";
	modules.threads[0].name = (uint8_t *)thread0_name;
	modules.threads[0].status = 19;
	modules.threads[0].main = scli;

	hal_threadContextInit(msg, &modules.threads[1].stack_pointer, &modules.threads[1].stack[THREAD_STACK_SIZE -1 ]);
	modules.threads[1].real_time_counter = 0;
	const char *thread1_name = "msg";
	modules.threads[1].name = (uint8_t *)thread1_name;
	modules.threads[1].status = 19;
	modules.threads[1].main = msg;

	hal_threadContextInit(task1, &modules.threads[2].stack_pointer, &modules.threads[2].stack[THREAD_STACK_SIZE -1 ]);
	modules.threads[2].real_time_counter = 0;
	const char *thread2_name = "task1";
	modules.threads[2].name = (uint8_t *)thread2_name;
	modules.threads[2].status = 12;
	modules.threads[2].main = task1;

	hal_threadContextInit(task2, &modules.threads[3].stack_pointer, &modules.threads[3].stack[THREAD_STACK_SIZE -1 ]);
	modules.threads[3].real_time_counter = 0;
	const char *thread3_name = "task2";
	modules.threads[3].name = (uint8_t *)thread3_name;
	modules.threads[3].status = 12;
	modules.threads[3].main = task2;
	// [/tag]
}

void initDrivers(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [autoCode_tag] drivers init
	const char *driver0_name = "hal_timerScheduler";
	modules.drivers[0]=(module_item_driver_t)
	{
		.name = (uint8_t *)driver0_name,
		.status = 1,
		.init = hal_timerSchedulerInit,
		.start = hal_timerSchedulerStart,
		.stop = hal_timerSchedulerStop
	};
	const char *driver1_name = "hal_timerRTC";
	modules.drivers[1]=(module_item_driver_t)
	{
		.name = (uint8_t *)driver1_name,
		.status = 1,
		.init = hal_timerRTCInit,
		.start = hal_timerRTCStart,
		.stop = hal_timerRTCStop
	};
	const char *driver2_name = "hal_i2c";
	modules.drivers[2]=(module_item_driver_t)
	{
		.name = (uint8_t *)driver2_name,
		.status = 1,
		.init = hal_i2cInit,
		.start = hal_i2cStart,
		.stop = hal_i2cStop
	};
	const char *driver3_name = "hal_usart";
	modules.drivers[3]=(module_item_driver_t)
	{
		.name = (uint8_t *)driver3_name,
		.status = 1,
		.init = hal_usartInit,
		.start = hal_usartStart,
		.stop = hal_usartStop
	};
	const char *driver4_name = "hal_lcd";
	modules.drivers[4]=(module_item_driver_t)
	{
		.name = (uint8_t *)driver4_name,
		.status = 2,
		.init = hal_lcdInit,
		.start = hal_lcdStart,
		.stop = hal_lcdStop
	};
	// [/tag]
}

// NOLINTEND
