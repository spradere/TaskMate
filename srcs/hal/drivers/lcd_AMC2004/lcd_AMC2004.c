/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file lcd_AMC2004.c
 * @brief arduinoMega hal_lcd_AMC2004_impl implementation.
 *
 */

#include "hal/drivers/lcd_AMC2004/lcd_AMC2004.h"

#include <util/delay.h>

#include "hal/mcu/atmega2560/i2c.h"

// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

static void lcdAMC2004SendCommand(uint8_t command);

#define LCDAMC2004_I2C_ADDR 0x3C // AiP31068L I2C address (Write mode)
#define LCDAMC2004_CMD 0x80 // Co=1 RS = 0, Write Command
#define LCDAMC2004_DATA 0x40 // Co=0 RS = 1, Write Data series
#define LCDAMC2004_RAW 4
#define LCDAMC2004_COL 20

void hal_lcdInit(void)
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
}

void hal_lcdStart(void)
{
	hal_lcdClear();
	hal_lcdWriteString("lcdAMC2004 ready");
}

void hal_lcdStop(void)
{
	// nothing to do.
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
