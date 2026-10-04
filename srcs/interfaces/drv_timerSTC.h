/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file drv_timerSTC.h
 * @brief Generic software time-counter timer driver interface declarations.
 */

#ifndef INTERFACES_DRV_TIMERSTC_H
#define INTERFACES_DRV_TIMERSTC_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include "interfaces/hal_drivers.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

typedef void (*hal_timerSTCCallback_t)(void);

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Apply a common software counter timer control command.
 * @return Resulting driver state.
 */
hal_driver_state_t hal_timerSTCControl(hal_driver_control_t command,
									   hal_driver_control_data_t *data);
/**
 * @brief Register the callback invoked by the software counter timer.
 * @param func_ptr Callback invoked from the timer interrupt.
 * @return Driver state after registration.
 */
hal_driver_state_t hal_timerSTCSetCallback(hal_timerSTCCallback_t func_ptr);

#endif // INTERFACES_DRV_TIMERSTC_H
