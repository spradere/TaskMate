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
 * @file modules_define.h
 * @brief thread and driver struct definitions
 *
 * @todo Nothing
 */

/**
 * @struct thread_item_t
 * @brief Represents a thread in TaskMate.
 */

#ifndef MODULES_DEFINE_H
#define MODULES_DEFINE_H

// run levels
#define RUN_NONE 0 /**< no start, could be started later via sCLI */
#define RUN_CORE 1 /**< start minimal critical system compoments, */
#define RUN_DRIVER 2 /**< start harware drivers*/
#define RUN_SERVICE 3 /**< start system servives*/
#define RUN_USER 4 /**< start user tasks*/
#define RUN_LEVEL_MASK 0xf8

// thread status bits
// bit [2 1 0 ] is run level
#define THREAD_START_AT_BOOT 3 /**< Automatic thread start at boot time */
#define THREAD_TYPE_USER 4 /**< user task*/
#define THREAD_TYPE_SYSTEM 5 /**< system service */
#define THREAD_DEAD 6 /**< Task can't be started */

// driver status bits
// bit [2 1 0 ] is run level
#define DRIVER_INIT_AT_BOOT 3 /**< Automatic driver initialization at boot time */
#define DRIVER_START_AT_BOOT 4 /**< Automatic driver start at boot time */
#define DRIVER_LOCK 5 /**< Driver locked for mutex purpose*/
#define DRIVER_DEAD 6 /**< Driver can't be started */
#define DRIVER_INIT 7 /**< Driver have been initialized */

#endif
