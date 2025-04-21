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


// thread table
#define THREAD_STACK_SIZE 256 /**< Thread stack size*/

// thread table
typedef struct
{
	uint8_t id; /**< Thread identifier */
	uint8_t *name; /**< Thread name */
	uint8_t run_level;

	void (*setStatus)(uint8_t); /**< Thread function for setting up status */
	uint8_t (*getStatus)(void); /**< Thread function for getting up status */
	void (*main)(void); /**< Thread main function for first start and resart */

	volatile uint16_t time_counter; /**< Thread's Real Time Counter */

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
	uint8_t id; /**< Driver indentifier */
	uint8_t *name; /**< Driver name */
	uint8_t run_level;

	void (*setStatus)(uint8_t); /**< Driver function for setting up status */
	uint8_t (*getStatus)(void); /**< Driver function for getting up status */

	void (*init)(void); /**< Initialize driver function  */
	void (*start)(void); /**< Start driver function  */
	void (*stop)(void); /**< Stop driver function  */

} driver_item_t;
