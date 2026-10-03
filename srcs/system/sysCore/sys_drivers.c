/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sys_drivers.c
 * @brief Static driver database implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "sys_drivers.h"

#include "interfaces/tm_modules.h"
#include "system/sysCore/sys_drivers_list.h"

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static mod_driver_item_t drivers[MOD_DRIVER_COUNT];

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

mod_driver_item_t *mod_driverGetPointer(uint8_t id) { return &drivers[id]; }

void mod_driversAlloc(void)
{
	// [autoCode_tag] drivers_alloc
#include "drivers_alloc.inc"
	// [/tag]
}
