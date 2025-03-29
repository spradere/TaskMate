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
 * @file driver_table.h
 * @brief driver struct and definitions
 *
 * @todo Nothing
 */

/**
 * @struct driver_table_t
 * @brief Represents a driver in TaskMate.
 */
 
// driver status bits
#define DRIVER_INIT_AT_BOOT 0 /**< Automatic driver initialization at boot time */
#define DRIVER_START_AT_BOOT 1 /**< Automatic driver start at boot time */
#define DRIVER_LOCK 2 /**< Driver locked for mutex purpose*/
#define DRIVER_DEAD 3 /**< Driver can't be started */
#define DRIVER_INIT 4 /**< Driver have been initialized */

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
	
} driver_table_t;
