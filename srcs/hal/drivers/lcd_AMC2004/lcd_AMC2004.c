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

#include "hal/public/i2c.h"
#include "hal/public/tmlibc.h"
#include "interfaces/macros.h"
#include "interfaces/modules_define.h"
#include "interfaces/runLevel_define.h"

// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

static void lcdAMC2004Clear(void);
static void lcdAMC2004SendCommand(uint8_t command);

static hal_driver_status_t lcd_status;

#define LCDAMC2004_I2C_ADDR 0x3C // AiP31068L I2C address (Write mode)
#define LCDAMC2004_CMD 0x80 // Co=1 RS = 0, Write Command
#define LCDAMC2004_DATA 0x40 // Co=0 RS = 1, Write Data series
#define LCDAMC2004_RAW 4
#define LCDAMC2004_COL 20

static uint8_t hal_lcdGetStatus(void)
{
	if( TM_GETBIT(lcd_status, DRV_BIT_ERROR) != 0 ) { return DRV_STATE_ERROR; }
	if( hal_i2cControl(DRV_CTRL_GETSTATUS, 0) != DRV_STATE_RUNNING ) { return DRV_STATE_ERROR; }
	if( TM_GETBIT(lcd_status, DRV_BIT_INIT) == 0 )
	{
		if( TM_GETBIT(lcd_status, DRV_BIT_START) == 0 ) { return DRV_STATE_OFF; }
		return DRV_STATE_ERROR;
	}
	if( TM_GETBIT(lcd_status, DRV_BIT_START) == 0 ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

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

	hal_lcdControl(DRV_CTRL_SETBIT, DRV_BIT_INIT);
	return 0;
}

static uint8_t hal_lcdStart(void)
{
	if( (hal_lcdControl(DRV_CTRL_GETBIT, DRV_BIT_INIT) == 0) ||
		(hal_lcdControl(DRV_CTRL_GETBIT, DRV_BIT_DEAD) != 0) )
	{
		return DRV_UNKNOWN;
	}

	lcdAMC2004Clear();
	hal_lcdControl(DRV_CTRL_SETBIT, DRV_BIT_START);
	return 0;
}

static uint8_t hal_lcdStop(void)
{
	// nothing to do.
	hal_lcdControl(DRV_CTRL_CLEARBIT, DRV_BIT_START);
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

static void lcdAMC2004Clear(void)
{
	lcdAMC2004SendCommand(0x01);
	_delay_ms(2);
}

uint8_t hal_lcdClear(void)
{
	if( hal_lcdGetStatus() != DRV_STATE_RUNNING ) { return DRV_STATE_ERROR; }
	lcdAMC2004Clear();
	return 0;
}

uint8_t hal_lcdSetCursor(uint8_t row, uint8_t col)
{
	if( hal_lcdGetStatus() != DRV_STATE_RUNNING ) { return DRV_STATE_ERROR; }
	const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	lcdAMC2004SendCommand(0x80 | (col + row_offsets[row]));
	return 0;
}

uint8_t hal_lcdWriteString(tm_string_t str)
{
	uint8_t index = 0;

	if( hal_lcdGetStatus() != DRV_STATE_RUNNING ) { return DRV_STATE_ERROR; }
	if( str.text == 0 ) { return DRV_STATE_ERROR; }

	hal_i2cCommStart(LCDAMC2004_I2C_ADDR, HAL_I2C_WRITE);
	hal_i2cWrite(LCDAMC2004_DATA);

	while( index < TM_STRING_SIZE_MAX )
	{
		char str_char = hal_string_getChar(&str, index);
		if( str_char == 0 ) { break; }
		hal_i2cWrite((uint8_t)str_char);
		index++;
	}
	hal_i2cCommStop();
	return 0;
}

uint8_t hal_lcdControl(uint8_t cmd, uint8_t val)
{
	switch( cmd )
	{
		case DRV_CTRL_INIT:
			return hal_lcdInit();
		case DRV_CTRL_START:
			return hal_lcdStart();
		case DRV_CTRL_STOP:
			return hal_lcdStop();
		case DRV_CTRL_RLSET:
			lcd_status &= (hal_driver_status_t)~RL_LEVEL_MASK;
			lcd_status |= val;
			return 0;
		case DRV_CTRL_RLGET:
			return lcd_status & RL_LEVEL_MASK;
		case DRV_CTRL_SETBIT:
			TM_SETBIT(lcd_status, val);
			return 0;
		case DRV_CTRL_CLEARBIT:
			TM_CLEARBIT(lcd_status, val);
			return 0;
		case DRV_CTRL_GETBIT:
			return TM_GETBIT(lcd_status, val);
		case DRV_CTRL_GETSTATUS:
			return hal_lcdGetStatus();
		default:
			return DRV_UNKNOWN;
	}
}

// NOLINTEND
