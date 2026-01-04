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

#ifndef HAL_I2C_H
#define HAL_I2C_H

#include <stdint.h>
#include <stdbool.h>

#define HAL_I2C_FREQ 100000UL // Standard mode 100 kHz

void hal_i2cInit(void);
void hal_i2cStart(void);
void hal_i2cStop(void);
uint8_t hal_i2cCommStart(uint8_t address);
void hal_i2cCommStop(void);
uint8_t hal_i2cWrite(uint8_t data);
uint8_t hal_i2cRead(bool ack);

#endif
