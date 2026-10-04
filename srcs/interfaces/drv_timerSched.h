/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file drv_timerSched.h
 * @brief Generic scheduler timer driver interface declarations.
 */

#ifndef INTERFACES_DRV_TIMERSCHED_H
#define INTERFACES_DRV_TIMERSCHED_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include "interfaces/hal_context.h"
#include "interfaces/hal_drivers.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

typedef hal_context_t *hal_timerSchedCallback_func_t(hal_context_t *context);
typedef hal_timerSchedCallback_func_t *hal_timerSchedCallback_ptr_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Apply a common scheduler timer control command.
 * @return Resulting driver state.
 */
hal_driver_state_t hal_timerSchedControl(hal_driver_control_t command,
										 hal_driver_control_data_t *data);
/**
 * @brief Register the scheduler timer callback.
 * @param func_ptr Callback invoked from the scheduler timer interrupt.
 * @return Driver state after registration.
 */
hal_driver_state_t hal_timerSchedSetCallback(hal_timerSchedCallback_ptr_t func_ptr);
/**
 * @brief Load the scheduler timer period.
 * @return Driver state after loading the timer.
 */
hal_driver_state_t hal_timerSchedLoad(void);

#endif // INTERFACES_DRV_TIMERSCHED_H
