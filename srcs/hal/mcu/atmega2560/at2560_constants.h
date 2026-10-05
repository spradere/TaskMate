/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file at2560_constants.h
 * @brief ATmega2560 peripheral constant declarations.
 *
 */

#ifndef ATMEGA2560_AT2560_CONSTANTS_H
#define ATMEGA2560_AT2560_CONSTANTS_H

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @name Peripheral bus rates
 * @brief Set USART baud and I2C bus frequency.
 * @{
 */
#define AT2560CONSTANTS_RATE_USART_baud 9600UL
#define AT2560CONSTANTS_FREQ_I2C_Hz 100000UL // Standard mode 100 kHz
/** @} */

#endif // ATMEGA2560_AT2560_CONSTANTS_H
