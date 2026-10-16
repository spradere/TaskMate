/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file drv_gpio.h
 * @brief Generic GPIO driver interface declarations.
 */

#ifndef INTERFACES_DRV_GPIO_H
#define INTERFACES_DRV_GPIO_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>

#include "interfaces/gpio_signals.h"
#include "interfaces/hal_drivers.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Apply a common GPIO driver control command.
 * @return Resulting driver state.
 */
hal_driver_state_t hal_gpioControl(hal_driver_control_t command, hal_driver_control_data_t *data);
/**
 * @brief Set a logical GPIO signal while the driver is running.
 */
void hal_gpioSignalWrite(gpio_signal_t signal, bool value);
/**
 * @brief Read a logical GPIO signal while the driver is running.
 * @return Current electrical level, or false if the driver is stopped.
 */
bool hal_gpioSignalRead(gpio_signal_t signal);

#endif // INTERFACES_DRV_GPIO_H
