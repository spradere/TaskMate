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
 * @file hal_i2c.c
 * @brief hal i2c serial comm
 *
 */

#include "hal/mcu/atmega2560/hal_i2c.h"
#include <avr/io.h>
#include <util/twi.h>

// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

#define HAL_I2C_TWBR_VALUE ((F_CPU / HAL_I2C_FREQ - 16) / 2)

void hal_i2cInit(void)
{
	TWBR = (uint8_t)HAL_I2C_TWBR_VALUE; // Set baud rate
	TWSR = 0x00; // Prescaler = 1
}

void hal_i2cStart(void)
{
	TWCR = (1 << TWEN); // Enable TWI
}

void hal_i2cStop(void)
{
	TWCR &= (uint8_t)~(1u << TWEN); // Stop TWI
}

uint8_t hal_i2cCommStart(uint8_t address)
{
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); // | (1 << TWEA);
	while( !(TWCR & (1 << TWINT)) );

	if( (TWSR & 0xF8) != TW_START )
	{
		return 1; // Error
	}

	TWDR = (address);
	TWCR = (1 << TWEN) | (1 << TWINT);
	while( !(TWCR & (1 << TWINT)) );

	if( (TWSR & 0xF8) != TW_MT_SLA_ACK )
	{
		return 2; // Error
	}
	return 0; // Success
}

void hal_i2cCommStop(void) { TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT); }

uint8_t hal_i2cWrite(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWEN) | (1 << TWINT);
	while( !(TWCR & (1 << TWINT)) );

	if( (TWSR & 0xF8) != TW_MT_DATA_ACK )
	{
		return 1; // Error
	}
	return 0; // Success
}

uint8_t hal_i2cRead(bool ack)
{
	if( ack )
		TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
	else
		TWCR = (1 << TWEN) | (1 << TWINT);

	while( !(TWCR & (1 << TWINT)) );

	uint8_t status = TWSR & 0xF8;

	if( ack && status != TW_MR_DATA_ACK ) return 0xFF;
	if( !ack && status != TW_MR_DATA_NACK ) return 0xFF;

	return TWDR;
}

// NOLINTEND
