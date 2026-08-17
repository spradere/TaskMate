/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file i2c.c
 * @brief i2c implementation.
 *
 */

#include "i2c.h"

#include <avr/io.h>
#include <util/twi.h>

#include "interfaces/macros.h"
#include "mcu_define.h" // get i2c frequency

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
	reg_setBit(TWCR, TWEN); // Enable TWI

	// address test
	// tm_syslog(TM_STR("[i2c] scan ...\n"));
	for( uint8_t adr = 0x00; adr != 0x7F; adr++ )
	{
		// start comm
		reg_setBit(TWCR, TWSTA, TWEN, TWINT);
		while( !reg_getBit(TWCR, TWINT) );

		if( (hal_i2cWrite((adr << 1))) == TW_MT_SLA_ACK )
		{
			// tm_syslog(TM_STR("\tfound SLA+W 0x%02x\n"), (adr));
		}

		hal_i2cCommStop();
	}
}

void hal_i2cStop(void)
{
	reg_clearBit(TWCR, TWEN); // Stop TWI
}

uint8_t hal_i2cCommStart(uint8_t address, bool rw)
{
	reg_setBit(TWCR, TWSTA, TWEN, TWINT);
	while( !reg_getBit(TWCR, TWINT) );

	if( (TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START) )
	{
		hal_i2cCommStop();
		return TW_STATUS;
	}

	return hal_i2cWrite((address << 1) | rw);
}

void hal_i2cCommStop(void)
{
	reg_setBit(TWCR, TWSTO, TWEN, TWINT);
}

uint8_t hal_i2cWrite(uint8_t data)
{
	TWDR = data;
	reg_setBit(TWCR, TWEN, TWINT);
	while( !reg_getBit(TWCR, TWINT) );

	return TW_STATUS;
}

uint8_t hal_i2cRead(uint8_t *data, bool ack)
{
	if( ack )
	{
		reg_setBit(TWCR, TWEN, TWINT, TWEA);
	}
	else
	{
		reg_setBit(TWCR, TWEN, TWINT);
	}

	while( !reg_getBit(TWCR, TWINT) );

	*data = TWDR;
	return TW_STATUS;
}

// NOLINTEND
