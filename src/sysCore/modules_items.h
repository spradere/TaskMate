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
 * @file modules_items.h
 * @brief thread and driver struct
 *
 */

#ifndef MODULES_ITEMS_H
#define MODULES_ITEMS_H

#include "sysCore/modules_define.h"
#include "hal/autoInclude_hal_system_critical.h" // get stack_word_t from selected arch

// thread
#define THREAD_STACK_SIZE 256

typedef struct
{
	const char *name;
	uint8_t status;

	void (*main)(void);

	volatile uint16_t real_time_counter;

	stack_word_t *stack_pointer;
	stack_word_t stack[THREAD_STACK_SIZE];

} module_item_thread_t;

// driver
typedef struct
{
	const char *name;
	uint8_t status;

	void (*init)(void);
	void (*start)(void);
	void (*stop)(void);

} module_item_driver_t;

#endif
