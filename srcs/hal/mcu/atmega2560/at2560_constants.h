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
#define USART_BAUD_RATE 9600
#define I2C_FREQ 100000UL // Standard mode 100 kHz
/** @} */

#endif // ATMEGA2560_AT2560_CONSTANTS_H
