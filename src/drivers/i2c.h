/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

 /**
 * @file i2c.h
 * @brief header of i2c communication
 * 
 * 
 */

#ifndef I2C_H
#define I2C_H

#include <stdint.h>


#define I2C_FREQ 100000UL // Standard mode 100 kHz
#define TWBR_VALUE ((F_CPU / I2C_FREQ - 16) / 2)

void i2cSetStatus(uint8_t);
uint8_t i2cGetStatus(void);
uint8_t *i2cGetName(void);

void i2cInit(void);
void i2cStart(void);
void i2cStop(void);

uint8_t i2cCommStart(uint8_t);
void i2cCommStop(void);
uint8_t i2cWrite(uint8_t);

#endif
