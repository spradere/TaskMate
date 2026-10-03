/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sys_drivers.h
 * @brief Static driver database declarations.
 */

#ifndef SYSCORE_SYS_DRIVERS_H
#define SYSCORE_SYS_DRIVERS_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

#include "interfaces/hal_drivers.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

typedef struct
{
	uint8_t address;

	hal_driver_state_t (*control)(hal_driver_control_t, hal_driver_control_data_t *);

} mod_driver_item_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

mod_driver_item_t *mod_driverGetPointer(uint8_t id);
void mod_driversAlloc(void);

#endif // SYSCORE_SYS_DRIVERS_H
