/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file drv_usart.h
 * @brief Generic USART driver interface declarations.
 */

#ifndef INTERFACES_DRV_USART_H
#define INTERFACES_DRV_USART_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

#include "interfaces/hal_drivers.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Apply a common USART driver control command.
 * @return Resulting driver state.
 */
hal_driver_state_t hal_usartControl(hal_driver_control_t command, hal_driver_control_data_t *data);
/**
 * @brief Read one byte from the USART receive buffer.
 * @param[out] data Receives a byte when available.
 * @return DRV_STATE_RUNNING if a byte was read; another state otherwise.
 */
hal_driver_state_t hal_usartRead(uint8_t *data);
/**
 * @brief Queue one byte for USART transmission.
 * @return Driver state after queuing the byte.
 */
hal_driver_state_t hal_usartWriteByte(uint8_t data);
/**
 * @brief Transmit queued USART bytes.
 * @return Driver state after sending queued bytes.
 */
hal_driver_state_t hal_usartSendTXBuffer(void);

#endif // INTERFACES_DRV_USART_H
