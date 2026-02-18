/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file hal_i2c.c
 * @brief hal i2c serial comm
 *
 */

#include "hal/mcu/atmega2560/hal_i2c.h"

#include <avr/io.h>

#include "hal/auto_hal_tmlibc.h"
#include "tm_libc/tm_syslog.h"

// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

#define I2C_TWBR_VALUE ((F_CPU / I2C_FREQ - 16) / 2)

void hal_i2cInit(void)
{
	TWBR = (uint8_t)I2C_TWBR_VALUE; // Set baud rate
	TWSR = 0x00; // Pre scaler = 1
}

void hal_i2cStart(void)
{
	TWCR = (uint8_t)(1u << TWEN); // Enable TWI

	// address test
	tm_syslog(TM_STR("[i2c] scan ...\n"));
	for( uint8_t adr = 0x00; adr != 0x7F; adr++ )
	{
		// start comm
		TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
		while( !(TWCR & (1 << TWINT)) );

		if( (hal_i2cWrite((adr << 1) | 0)) == TW_MT_SLA_ACK )
		{
			tm_syslog(TM_STR("[i2c] found SLA+W 0x%x\n"), (adr));
		}

		hal_i2cCommStop();
	}
}

void hal_i2cStop(void)
{
	TWCR &= (uint8_t)~(1u << TWEN); // Stop TWI
}

uint8_t hal_i2cCommStart(uint8_t address, bool rw)
{
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while( !(TWCR & (1 << TWINT)) );

	if( (TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START) )
	{
		hal_i2cCommStop();
		return TW_STATUS;
	}

	return hal_i2cWrite((address << 1) | rw);
}

void hal_i2cCommStop(void) { TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT); }

uint8_t hal_i2cWrite(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWEN) | (1 << TWINT);
	while( !(TWCR & (1 << TWINT)) );

	return TW_STATUS;
}

uint8_t hal_i2cRead(uint8_t *data, bool ack)
{
	if( ack )
		TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
	else
		TWCR = (1 << TWEN) | (1 << TWINT);

	while( !(TWCR & (1 << TWINT)) );

	*data = TWDR;
	return TW_STATUS;
}

// NOLINTEND
