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
 * @file status_bits.h
 * @brief status bits definition for tasks and drivers
 *
 * @todo Nothing
 */

// task status bits
#define TASK_START_AT_BOOT 0 /**< Automatic task start at boot time */
#define TASK_TYPE_USER 1 /**< user task*/
#define TASK_TYPE_SYSTEM 2 /**< system service */
#define TASK_DEAD 3 /**< Task can't be started */

// driver status bits
#define DRIVER_INIT_AT_BOOT 0 /**< Automatic driver initialization at boot time */
#define DRIVER_START_AT_BOOT 1 /**< Automatic driver start at boot time */
#define DRIVER_LOCK 2 /**< Driver locked for mutex purpose*/
#define DRIVER_DEAD 3 /**< Driver can't be started */
#define DRIVER_INIT 4 /**< Driver have been initialized */
