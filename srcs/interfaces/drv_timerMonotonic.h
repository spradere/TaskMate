/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file drv_timerMonotonic.h
 * @brief Generic monotonic timer driver interface declarations.
 */

#ifndef INTERFACES_DRV_TIMERMONOTONIC_H
#define INTERFACES_DRV_TIMERMONOTONIC_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

#include "interfaces/hal_drivers.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Apply a common monotonic timer control command.
 * @return Resulting driver state.
 */
hal_driver_state_t hal_timerMonotonicControl(hal_driver_control_t command,
											 hal_driver_control_data_t *data);

/**
 * @brief Read elapsed 100 us ticks since boot, modulo 2^32.
 * The count advances only while the timer is running. The read is atomic on 8-bit MCUs.
 * @return Current tick count.
 */
uint32_t hal_timerMonotonicRead(void);

#endif // INTERFACES_DRV_TIMERMONOTONIC_H
