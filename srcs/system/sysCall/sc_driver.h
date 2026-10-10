/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sc_driver.h
 * @brief Driver and HAL syscall declarations.
 */

#ifndef SYSCALL_SC_DRIVER_H
#define SYSCALL_SC_DRIVER_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <stdint.h>

#include "interfaces/drv_rtc.h"
#include "interfaces/error_catalog.h"
#include "interfaces/tm_mod.h"
#include "interfaces/tm_string.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/* -----------------------------------------------
 * Driver life cycle
 * ---------------------------------------------*/

/**
 * @brief Get the number of configured drivers.
 */
mod_count_t sc_driverGetCount(void);
/**
 * @brief Read metadata and status for a driver identifier.
 * @param id Driver identifier below sc_driverGetCount().
 * @param[out] name Receives the driver name.
 * @param[out] run_level Receives the configured run level.
 * @param[out] status_bits Receives driver status bits.
 * @return true if driver information was retrieved.
 */
bool sc_driverGetInfo(mod_count_t id, const tm_string_t **name, uint8_t *run_level,
					  uint8_t *status_bits);
/**
 * @brief Initialize a driver by name.
 * @return true if the named driver was initialized.
 */
bool sc_driverInit(const char *name);
/**
 * @brief Start a driver by name.
 * @return true if the named driver was started.
 */
bool sc_driverStart(const char *name);
/**
 * @brief Stop a driver by name.
 * @return true if the named driver was stopped.
 */
bool sc_driverStop(const char *name);
/**
 * @brief Start drivers assigned to a run level.
 */
void sc_driverRunLevelStart(uint8_t run_level);
/**
 * @brief Check whether all drivers in a run level are ready.
 * @return true if all drivers in the run level are ready.
 */
bool sc_driverRunLevelIsReady(uint8_t run_level);

/* -----------------------------------------------
 * LCD operations
 * ---------------------------------------------*/

/**
 * @brief Clear the LCD through the driver interface.
 * @return ERR_NO_ERROR on success; an error code otherwise.
 */
err_codes_t sc_lcdClear(void);
/**
 * @brief Write a string at an LCD position.
 * @param str RAM or ROM text to display.
 * @param row Display row.
 * @param col Display column.
 * @return ERR_NO_ERROR on success; an error code otherwise.
 */
err_codes_t sc_lcdWriteString(tm_string_t str, uint8_t row, uint8_t col);

/* -----------------------------------------------
 * RTC operations
 * ---------------------------------------------*/

/**
 * @brief Read the current RTC time.
 * @param[out] time Receives the current RTC time.
 * @return ERR_NO_ERROR on success; an error code otherwise.
 */
err_codes_t sc_rtcRead(hal_rtc_time_t *time);
/**
 * @brief Set the RTC time.
 * @param time RTC time to write.
 * @return ERR_NO_ERROR on success; an error code otherwise.
 */
err_codes_t sc_rtcWrite(const hal_rtc_time_t *time);
/**
 * @brief Save the current RTC time as startup time.
 * @return ERR_NO_ERROR on success; an error code otherwise.
 */
err_codes_t sc_rtcSaveStartupTime(void);
/**
 * @brief Read the saved startup time.
 * @param[out] time Receives the saved startup time.
 * @return ERR_NO_ERROR on success; an error code otherwise.
 */
err_codes_t sc_rtcGetStartupTime(hal_rtc_time_t *time);

/* -----------------------------------------------
 * I2C operations
 * ---------------------------------------------*/

/**
 * @brief Scan the I2C bus for configured devices.
 * @return ERR_NO_ERROR on success; an error code otherwise.
 */
err_codes_t sc_i2cScan(void);

/* -----------------------------------------------
 * USART operations
 * ---------------------------------------------*/

/**
 * @brief Read one byte from the USART receive buffer.
 * @param[out] data Receives a byte when available.
 * @return ERR_NO_ERROR on success; an error code otherwise.
 */
err_codes_t sc_usartRead(uint8_t *data);

#endif // SYSCALL_SC_DRIVER_H
