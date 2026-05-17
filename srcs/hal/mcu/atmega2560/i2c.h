/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file i2c.h
 * @brief i2c header declarations.
 *
 */

#ifndef ATMEGA2560_I2C_H
#define ATMEGA2560_I2C_H

#include <stdbool.h>
#include <stdint.h>

// #define I2C_FREQ 100000UL // Standard mode 100 kHz
#define HAL_I2C_READ 1
#define HAL_I2C_WRITE 0
#define HAL_I2C_ACK 1
#define HAL_I2C_NACK 0

void hal_i2cInit(void);
void hal_i2cStart(void);
void hal_i2cStop(void);
uint8_t hal_i2cCommStart(uint8_t address, bool rw);
void hal_i2cCommStop(void);
uint8_t hal_i2cWrite(uint8_t data);
uint8_t hal_i2cRead(uint8_t *data, bool ack);

#endif // ATMEGA2560_I2C_H
