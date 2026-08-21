/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file lcd_AMC2004.c
 * @brief lcd amc2004 implementation.
 *
 */

#include "lcd_AMC2004.h"

#include <util/delay.h>

#include "hal/mcu/atmega2560/i2c.h"
#include "interfaces/drivers.h"
#include "interfaces/macros.h"

// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

static void lcdAMC2004SendCommand(uint8_t command);
static uint8_t hal_lcdInit(void);
static uint8_t hal_lcdStart(void);
static uint8_t hal_lcdStop(void);

static hal_driver_status_t lcd_status;

uint8_t hal_lcdControl(uint8_t cmd, uint8_t val)
{
	switch( cmd )
	{
		case TM_DRIVER_CTRL_INIT:
			hal_lcdInit();
			break;
		case TM_DRIVER_CTRL_START:
			hal_lcdStart();
			break;
		case TM_DRIVER_CTRL_STOP:
			hal_lcdStop();
			break;
		case TM_DRIVER_STATUS_RLSET:
			lcd_status &= (hal_driver_status_t)~TM_DRIVER_RL_MASK;
			lcd_status |= val;
			return 0;
		case TM_DRIVER_STATUS_RLGET:
			return lcd_status &= TM_DRIVER_RL_MASK;
		case TM_DRIVER_STATUS_SETBIT:
			TM_SETBIT(lcd_status, val);
			return 0;
		case TM_DRIVER_STATUS_CLEARBIT:
			TM_CLEARBIT(lcd_status, val);
			return 0;
		case TM_DRIVER_STATUS_GETBIT:
			return TM_GETBIT(lcd_status, val);
		default:
			return TM_DRIVER_UNKNOW;
	}
	return 0;
}

#define LCDAMC2004_I2C_ADDR 0x3C // AiP31068L I2C address (Write mode)
#define LCDAMC2004_CMD 0x80 // Co=1 RS = 0, Write Command
#define LCDAMC2004_DATA 0x40 // Co=0 RS = 1, Write Data series
#define LCDAMC2004_RAW 4
#define LCDAMC2004_COL 20

static uint8_t hal_lcdInit(void)
{
	_delay_ms(50); // Wait for LCD to power up

	lcdAMC2004SendCommand(0x38); // Function Set: 8-bit mode, 2 lines, 5x8 dots
	_delay_us(110);
	lcdAMC2004SendCommand(0x0C); // Display ON, Cursor OFF, Blink OFF
	_delay_us(110);
	lcdAMC2004SendCommand(0x01); // Clear Display
	_delay_ms(11);
	lcdAMC2004SendCommand(0x06); // Entry Mode: Cursor moves right, no shift
	_delay_us(110);

	hal_lcdControl(TM_DRIVER_STATUS_SETBIT, TM_DRIVER_BIT_INIT);
	return 0;
}

static uint8_t hal_lcdStart(void)
{
	if( (hal_lcdControl(TM_DRIVER_STATUS_GETBIT, TM_DRIVER_BIT_INIT) == 0) ||
		(hal_lcdControl(TM_DRIVER_STATUS_GETBIT, TM_DRIVER_BIT_DEAD) != 0) )
	{
		return TM_DRIVER_UNKNOW;
	}

	hal_lcdClear();
	hal_lcdWriteString("lcdAMC2004 ready");

	hal_lcdControl(TM_DRIVER_STATUS_SETBIT, TM_DRIVER_BIT_START);
	return 0;
}

static uint8_t hal_lcdStop(void)
{
	// nothing to do.
	hal_lcdControl(TM_DRIVER_STATUS_CLEARBIT, TM_DRIVER_BIT_START);
	return 0;
}

void lcdAMC2004SendCommand(uint8_t command)
{
	hal_i2cCommStart(LCDAMC2004_I2C_ADDR, HAL_I2C_WRITE);
	hal_i2cWrite(LCDAMC2004_CMD); // Control byte: RS=0, RW=0
	hal_i2cWrite(command);
	hal_i2cCommStop();
	_delay_us(200); // Small delay for LCD to process the command
}

void hal_lcdClear(void)
{
	lcdAMC2004SendCommand(0x01);
	_delay_ms(2);
}

void hal_lcdSetCursor(uint8_t row, uint8_t col)
{
	const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	lcdAMC2004SendCommand(0x80 | (col + row_offsets[row]));
}

void hal_lcdWriteString(const char *str)
{
	hal_i2cCommStart(LCDAMC2004_I2C_ADDR, HAL_I2C_WRITE);
	hal_i2cWrite(LCDAMC2004_DATA);

	while( *str ) { hal_i2cWrite((uint8_t)*str++); }
	hal_i2cCommStop();
}

// NOLINTEND
