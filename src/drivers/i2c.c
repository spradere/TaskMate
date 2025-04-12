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
 * @file i2c.c
 * @brief implementation of i2c communication
 *
 * @todo Handle hardware errors
 */

#include <avr/io.h>
#include <util/twi.h>
#include "sysCore/TaskMate_public.h"
#include "drivers/i2c.h"

// status
uint8_t i2c_status = 0;

void i2cSetStatus(uint8_t status) { i2c_status = status; }
uint8_t i2cGetStatus(void) { return i2c_status; }

void i2cInit(void)
{
	TWBR = (uint8_t)TWBR_VALUE; // Set baud rate
	TWSR = 0x00; // Prescaler = 1
}

void i2cStart(void)
{
	// Enable TWI
	TWCR = (1 << TWEN);
}

void i2cStop(void)
{
	// Stop TWI
	TWCR &= ~(1 << TWEN);
}

uint8_t i2cCommStart(uint8_t address)
{
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); // | (1 << TWEA);
	while (!(TWCR & (1 << TWINT)));

	if ((TWSR & 0xF8) != TW_START)
	{
		return 1; // Error
	}

	TWDR = (address);
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));

	if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
	{
		return 2; // Error
	}
	return 0; // Success
}

void i2cCommStop(void) { TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT); }

uint8_t i2cWrite(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));

	if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
	{
		return 1; // Error
	}
	return 0; // Success
}
