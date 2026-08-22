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

#include "interfaces/drivers.h"
#include "interfaces/macros.h"
#include "mcu_define.h" // get i2c frequency
#include "tm_libc/tm_syslog.h"

// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

#define I2C_TWBR_VALUE ((F_CPU / I2C_FREQ - 16) / 2)

static hal_driver_status_t i2c_status;

static uint8_t hal_i2cInit(void)
{
	TWBR = (uint8_t)I2C_TWBR_VALUE; // Set baud rate
	TWSR = 0x00; // Pre scaler = 1

	hal_i2cControl(DRV_CTRL_SETBIT, DRV_BIT_INIT);
	return 0;
}

static uint8_t hal_i2cStart(void)
{
	if( (hal_i2cControl(DRV_CTRL_GETBIT, DRV_BIT_INIT) == 0) ||
		(hal_i2cControl(DRV_CTRL_GETBIT, DRV_BIT_DEAD) != 0) )
	{
		return DRV_UNKNOW;
	}

	TWCR = (uint8_t)(1u << TWEN); // Enable TWI

	// address test
	tm_syslog(TM_STR("[i2c] scan ...\n"));
	for( uint8_t adr = 0x00; adr != 0x7F; adr++ )
	{
		// start comm
		TM_WRITEBIT(TWCR, TWSTA, TWEN, TWINT);
		while( !(TM_GETBIT(TWCR, TWINT)) );

		if( (hal_i2cWrite((adr << 1))) == TW_MT_SLA_ACK )
		{
			tm_syslog(TM_STR("\tfound SLA+W 0x%02x\n"), (adr));
		}

		hal_i2cCommStop();
	}

	hal_i2cControl(DRV_CTRL_SETBIT, DRV_BIT_START);
	return 0;
}

static uint8_t hal_i2cStop(void)
{
	TM_CLEARBIT(TWCR, TWEN);

	hal_i2cControl(DRV_CTRL_CLEARBIT, DRV_BIT_START);
	return 0;
}

uint8_t hal_i2cCommStart(uint8_t address, bool rw)
{
	TM_WRITEBIT(TWCR, TWSTA, TWEN, TWINT);
	while( !(TM_GETBIT(TWCR, TWINT)) );
	if( (TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START) )
	{
		hal_i2cCommStop();
		return TW_STATUS;
	}

	return hal_i2cWrite((address << 1) | rw);
}

void hal_i2cCommStop(void) { TM_WRITEBIT(TWCR, TWSTO, TWEN, TWINT); }

uint8_t hal_i2cWrite(uint8_t data)
{
	TWDR = data;
	TM_WRITEBIT(TWCR, TWEN, TWINT);
	while( !(TM_GETBIT(TWCR, TWINT)) );

	return TW_STATUS;
}

uint8_t hal_i2cRead(uint8_t *data, bool ack)
{
	if( ack ) { TM_WRITEBIT(TWCR, TWEN, TWINT, TWEA); }
	else { TM_WRITEBIT(TWCR, TWEN, TWINT); }
	while( !(TM_GETBIT(TWCR, TWINT)) );

	*data = TWDR;
	return TW_STATUS;
}

uint8_t hal_i2cControl(uint8_t cmd, uint8_t val)
{
	switch( cmd )
	{
		case DRV_CTRL_INIT:
			return hal_i2cInit();
		case DRV_CTRL_START:
			return hal_i2cStart();
		case DRV_CTRL_STOP:
			return hal_i2cStop();
		case DRV_CTRL_RLSET:
			i2c_status &= (hal_driver_status_t)~DRV_RL_MASK;
			i2c_status |= val;
			return 0;
		case DRV_CTRL_RLGET:
			return i2c_status & DRV_RL_MASK;
		case DRV_CTRL_SETBIT:
			TM_SETBIT(i2c_status, val);
			return 0;
		case DRV_CTRL_CLEARBIT:
			TM_CLEARBIT(i2c_status, val);
			return 0;
		case DRV_CTRL_GETBIT:
			return TM_GETBIT(i2c_status, val);
		default:
			return DRV_UNKNOW;
	}
}

// NOLINTEND
