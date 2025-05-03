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

// thread status bits
// bit [2 1 0 ] is run level
#define THREAD_TYPE_USER 3 /**< user task*/
#define THREAD_TYPE_SYSTEM 4 /**< system service */
#define THREAD_DEAD 5 /**< Task can't be started */

// driver status bits
// bit [2 1 0 ] is run level
#define DRIVER_LOCK 3 /**< Driver locked for mutex purpose*/
#define DRIVER_DEAD 4 /**< Driver can't be started */
#define DRIVER_INIT 5 /**< Driver have been initialized */

#endif
