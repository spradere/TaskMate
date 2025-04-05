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

#include "sysCore/status_bits.h"

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
