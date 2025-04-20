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
 * @file module_t.h
 * @brief thread and driver struct and definitions
 *
 * @todo Nothing
 */

/**
 * @struct thread_item_t
 * @brief Represents a thread in TaskMate.
 */


// task table
#define THREAD_STACK_SIZE 256 /**< Thread stack size*/

// task table
typedef struct
{
	uint8_t thread_id; /**< Thread identifier */
	uint8_t *thread_name; /**< Thread name */

	void (*setStatus)(uint8_t); /**< Thread function for setting up status */
	uint8_t (*getStatus)(void); /**< Thread function for getting up status */
	void (*main)(void); /**< Thread main function for first start and resart */

	volatile uint16_t task_RTC; /**< Task's Real Time Counter */

	uint8_t *stack_pointer; /**< Thread stack Pointer, pointer to stack array items*/
	uint8_t stack[THREAD_STACK_SIZE]; /**< Thread stack array */
} thread_item_t;

/**
 * @struct driver_item_t
 * @brief Represents a driver in TaskMate.
 */


// driver table
typedef struct
{
	uint8_t driver_id; /**< Driver indentifier */
	uint8_t *driver_name; /**< Driver name */

	void (*setStatus)(uint8_t); /**< Driver function for setting up status */
	uint8_t (*getStatus)(void); /**< Driver function for getting up status */

	void (*init)(void); /**< Initialize driver function  */
	void (*start)(void); /**< Start driver function  */
	void (*stop)(void); /**< Stop driver function  */

} driver_item_t;
