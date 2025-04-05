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
 * @file task_table.h
 * @brief task struct and definitions
 *
 * @todo Nothing
 */

/**
 * @struct task_table_t
 * @brief Represents a task in TaskMate.
 */

#include "sysCore/status_bits.h"

// task table
#define TASK_STACK_SIZE 256 /**< Task stack size*/

// task table
typedef struct
{
	uint8_t task_id; /**< Task identifier */
	uint8_t *task_name; /**< Task name */

	void (*setStatus)(uint8_t); /**< Task function for setting up status */
	uint8_t (*getStatus)(void); /**< Task function for getting up status */
	void (*main)(void); /**< Task main function for first start and resart */

	volatile uint16_t task_RTC; /**< Task's Real Time Counter */

	uint8_t *stack_pointer; /**< Task stack Pointer, pointer to stack array items*/
	uint8_t stack[TASK_STACK_SIZE]; /**< Task stack array */
} task_table_t;
