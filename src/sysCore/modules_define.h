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
 * @file modules_define.h
 * @brief thread and driver definitions
 *
 * @todo Nothing
 */

/**
 * @struct thread_item_t
 * @brief Represents a thread in TaskMate.
 */

#ifndef MODULES_DEFINE_H
#define MODULES_DEFINE_H

// modules
#define MODULES_TASKS_COUNT_MAX 128
#define MODULES_SERVICES_COUNT_MAX 128
#define MODULES_DRIVERS_COUNT_MAX 256

#define MODULES_DRIVERS_ID 0
#define MODULES_SERVICES_ID 1
#define MODULES_TASKS_ID 2

#define MODULES_TYPE_COUNT 3
#define MODULES_NAME_SIZE_MAX 32

// thread status bits
// bit [2 1 0 ] is run level
#define MODULES_THREAD_TYPE_USER 3 /**< user task*/
#define MODULES_THREAD_TYPE_SYSTEM 4 /**< system service */
#define MODULES_THREAD_DEAD 5 /**< Task can't be started */

// driver status bits
// bit [2 1 0 ] is run level
#define MODULES_DRIVER_LOCK 3 /**< Driver locked for mutex purpose*/
#define MODULES_DRIVER_DEAD 4 /**< Driver can't be started */
#define MODULES_DRIVER_INIT 5 /**< Driver have been initialized */

#endif
