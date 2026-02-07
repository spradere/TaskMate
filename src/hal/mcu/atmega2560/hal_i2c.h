/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file hal_i2c.h
 * @brief header hal i2c serial comm
 *
 */

// @hal_user

#ifndef HAL_I2C_H
#define HAL_I2C_H

#include <stdbool.h>
#include <stdint.h>
#include <util/twi.h>

#define I2C_FREQ 100000UL // Standard mode 100 kHz
#define I2C_READ 1
#define I2C_WRITE 0
#define I2C_ACK 1
#define I2C_NACK 0

void hal_i2cInit(void);
void hal_i2cStart(void);
void hal_i2cStop(void);
uint8_t hal_i2cCommStart(uint8_t address, bool rw);
void hal_i2cCommStop(void);
uint8_t hal_i2cWrite(uint8_t data);
uint8_t hal_i2cRead(uint8_t *data, bool ack);

#endif
