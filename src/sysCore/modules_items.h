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
 * @todo Nothing
 */

/**
 * @struct thread_item_t
 * @brief Represents a thread in TaskMate.
 */

#ifndef MODULES_ITEMS_H
#define MODULES_ITEMS_H

#include "sysCore/modules_define.h"
#include "hal/autoInclude_hal.h" // get stack_word_t from selected arch

// thread
#define THREAD_STACK_SIZE 256 /**< Thread stack size*/

typedef struct
{
	uint8_t *name; /**< Thread name */
	uint8_t status; /**< flag | run level */

	void (*main)(void); /**< Thread main function for first start and resart */

	volatile uint16_t real_time_counter; /**< Thread's Time Counter */

	stack_word_t *stack_pointer; /**< Thread stack Pointer, pointer to stack array items*/
	stack_word_t stack[THREAD_STACK_SIZE]; /**< Thread stack array */

} module_item_thread_t;

/**
 * @struct driver_item_t
 * @brief Represents a driver in TaskMate.
 */

// driver
typedef struct
{
	uint8_t *name; /**< Driver name */
	uint8_t status; /**< flag | run level */

	void (*init)(void); /**< Initialize driver function  */
	void (*start)(void); /**< Start driver function  */
	void (*stop)(void); /**< Stop driver function  */

} module_item_driver_t;

#endif
