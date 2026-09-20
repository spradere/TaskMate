/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_drivers.h
 * @brief Common HAL driver types.
 */

#ifndef INTERFACES_HAL_DRIVERS_H
#define INTERFACES_HAL_DRIVERS_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <stdint.h>

#include "interfaces/error_catalog.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

typedef enum
{
	DRV_CTRL_INIT,
	DRV_CTRL_START,
	DRV_CTRL_STOP,
	DRV_CTRL_RLSET,
	DRV_CTRL_RLGET,
	DRV_CTRL_SETBIT,
	DRV_CTRL_CLEARBIT,
	DRV_CTRL_GETBIT,
	DRV_CTRL_GETSTATUS,
	DRV_CTRL_GETLASTERROR
} hal_driver_control_t;

typedef uint8_t hal_driver_status_t;

// bits [2:0] contain the run level
typedef enum
{
	DRV_BIT_INIT = 3,
	DRV_BIT_START,
	DRV_BIT_ERROR,
	DRV_BIT_DEAD
} hal_driver_status_bit_t;

typedef enum
{
	DRV_STATE_OFF,
	DRV_STATE_INITIALIZED,
	DRV_STATE_RUNNING,
	DRV_STATE_ERROR,
	DRV_STATE_DEAD
} hal_driver_state_t;

typedef union
{
	uint8_t run_level;
	hal_driver_status_bit_t status_bit;
	bool bit_value;
	err_codes_t error;
} hal_driver_control_data_t;

#endif // INTERFACES_HAL_DRIVERS_H
