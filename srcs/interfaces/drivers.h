/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file drivers.h
 * @brief drivers header declarations.
 */

#ifndef INTERFACES_DRIVERS_H
#define INTERFACES_DRIVERS_H

// driver control
#define TM_DRIVER_CTRL_INIT 1
#define TM_DRIVER_CTRL_START 2
#define TM_DRIVER_CTRL_STOP 3
#define TM_DRIVER_STATUS_RLSET 4
#define TM_DRIVER_STATUS_RLSET 5
#define TM_DRIVER_STATUS_SETBIT 6
#define TM_DRIVER_STATUS_CLEARBIT 7
#define TM_DRIVER_STATUS_GETBIT 8
#define TM_DRIVER_UNKNOW 99

// driver status bits
typedef uint8_t hal_driver_status_t;

// bit [2 1 0 ] is run level
#define TM_DRIVER_RL_MASK 0x07
#define TM_DRIVER_BIT_INIT 3
#define TM_DRIVER_BIT_START 4
#define TM_DRIVER_BIT_DEAD 5

#endif // INTERFACES_DRIVERS_H
