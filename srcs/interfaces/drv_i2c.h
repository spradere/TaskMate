/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file drv_i2c.h
 * @brief Generic I2C driver interface declarations.
 */

#ifndef INTERFACES_DRV_I2C_H
#define INTERFACES_DRV_I2C_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

#include "interfaces/hal_drivers.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Selects I2C transfer direction.
 */
typedef enum
{
	HAL_I2C_WRITE,
	HAL_I2C_READ
} hal_i2c_direction_t;

/**
 * @brief Selects the acknowledge bit sent after an I2C read.
 */
typedef enum
{
	HAL_I2C_NACK,
	HAL_I2C_ACK
} hal_i2c_ack_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Apply a common I2C driver control command.
 * @param command Common driver control command.
 * @param[in,out] data Command input or output, when required.
 * @return Resulting driver state.
 */
hal_driver_state_t hal_i2cControl(hal_driver_control_t command, hal_driver_control_data_t *data);
/**
 * @brief Find the next I2C device and report its address.
 * @param[out] address Receives the discovered bus address.
 * @return DRV_STATE_RUNNING if a device was found; another state otherwise.
 */
hal_driver_state_t hal_i2cScan(uint8_t *address);
/**
 * @brief Start an I2C transfer to the selected address.
 * @param address Seven-bit I2C device address.
 * @param direction Transfer direction.
 * @return Driver state after the start condition.
 */
hal_driver_state_t hal_i2cCommStart(uint8_t address, hal_i2c_direction_t direction);
/**
 * @brief Stop the current I2C transfer.
 * @return Driver state after the stop condition.
 */
hal_driver_state_t hal_i2cCommStop(void);
/**
 * @brief Write one byte in the current I2C transfer.
 * @return Driver state after sending the byte.
 */
hal_driver_state_t hal_i2cWrite(uint8_t data);
/**
 * @brief Read one byte and send the selected acknowledge bit.
 * @param[out] data Receives the byte on success.
 * @param ack Acknowledge bit to send after the byte.
 * @return Driver state after receiving the byte.
 */
hal_driver_state_t hal_i2cRead(uint8_t *data, hal_i2c_ack_t ack);

#endif // INTERFACES_DRV_I2C_H
