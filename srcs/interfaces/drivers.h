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

#include <stdint.h>

// driver control
#define DRV_CTRL_INIT 1
#define DRV_CTRL_START 2
#define DRV_CTRL_STOP 3
#define DRV_CTRL_RLSET 4
#define DRV_CTRL_RLGET 5
#define DRV_CTRL_SETBIT 6
#define DRV_CTRL_CLEARBIT 7
#define DRV_CTRL_GETBIT 8
#define DRV_CTRL_GETSTATUS 9
#define DRV_UNKNOW 99

// driver status bits
typedef uint8_t hal_driver_status_t;

// bit [2 1 0 ] is run level
#define DRV_RL_MASK 0x07
#define DRV_BIT_INIT 3
#define DRV_BIT_START 4
#define DRV_BIT_ERROR 5
#define DRV_BIT_DEAD 6

// driver status
#define DRV_STATE_OFF 1
#define DRV_STATE_INITIALIZED 2
#define DRV_STATE_RUNNING 3
#define DRV_STATE_ERROR 4

#endif // INTERFACES_DRIVERS_H
