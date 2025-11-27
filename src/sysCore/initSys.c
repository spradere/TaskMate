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
 */


#include "hal/hal_api.h"

#include "sysCore/autoInclude_system.h"
#include "sysCore/modules.h"
#include "sysCore/initSys.h"


// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

void initThreads(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [autoCode_tag] threads init
	module_item_thread_t *mod_t;

	mod_t = moduleThreadGetPointer(0);

	hal_threadContextInit(scli, &(mod_t->stack_pointer), &(mod_t->stack[THREAD_STACK_SIZE -1 ]) );
	mod_t->real_time_counter = 0;
	const char *thread0_name = "scli";
	mod_t->name = (uint8_t *)thread0_name;
	mod_t->status = 19;
	mod_t->main = scli;

	mod_t = moduleThreadGetPointer(1);

	hal_threadContextInit(msg, &(mod_t->stack_pointer), &(mod_t->stack[THREAD_STACK_SIZE -1 ]) );
	mod_t->real_time_counter = 0;
	const char *thread1_name = "msg";
	mod_t->name = (uint8_t *)thread1_name;
	mod_t->status = 19;
	mod_t->main = msg;

	mod_t = moduleThreadGetPointer(2);

	hal_threadContextInit(task1, &(mod_t->stack_pointer), &(mod_t->stack[THREAD_STACK_SIZE -1 ]) );
	mod_t->real_time_counter = 0;
	const char *thread2_name = "task1";
	mod_t->name = (uint8_t *)thread2_name;
	mod_t->status = 12;
	mod_t->main = task1;

	mod_t = moduleThreadGetPointer(3);

	hal_threadContextInit(task2, &(mod_t->stack_pointer), &(mod_t->stack[THREAD_STACK_SIZE -1 ]) );
	mod_t->real_time_counter = 0;
	const char *thread3_name = "task2";
	mod_t->name = (uint8_t *)thread3_name;
	mod_t->status = 12;
	mod_t->main = task2;
	// [/tag]
}

void initDrivers(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [autoCode_tag] drivers init
	module_item_driver_t *mod_d;

	mod_d = moduleDriverGetPointer(0);
	const char *driver0_name = "hal_timerScheduler";
	*(mod_d) = (module_item_driver_t)
	{
		.name = (uint8_t *)driver0_name,
		.status = 1,
		.init = hal_timerSchedulerInit,
		.start = hal_timerSchedulerStart,
		.stop = hal_timerSchedulerStop
	};

	mod_d = moduleDriverGetPointer(1);
	const char *driver1_name = "hal_timerRTC";
	*(mod_d) = (module_item_driver_t)
	{
		.name = (uint8_t *)driver1_name,
		.status = 1,
		.init = hal_timerRTCInit,
		.start = hal_timerRTCStart,
		.stop = hal_timerRTCStop
	};

	mod_d = moduleDriverGetPointer(2);
	const char *driver2_name = "hal_i2c";
	*(mod_d) = (module_item_driver_t)
	{
		.name = (uint8_t *)driver2_name,
		.status = 1,
		.init = hal_i2cInit,
		.start = hal_i2cStart,
		.stop = hal_i2cStop
	};

	mod_d = moduleDriverGetPointer(3);
	const char *driver3_name = "hal_usart";
	*(mod_d) = (module_item_driver_t)
	{
		.name = (uint8_t *)driver3_name,
		.status = 1,
		.init = hal_usartInit,
		.start = hal_usartStart,
		.stop = hal_usartStop
	};

	mod_d = moduleDriverGetPointer(4);
	const char *driver4_name = "hal_lcd";
	*(mod_d) = (module_item_driver_t)
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
