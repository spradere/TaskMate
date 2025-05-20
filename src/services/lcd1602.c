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
 * @file lcd1602.c
 * @brief implementation of lcd display 1602 2x16
 *
 * @todo Nothing
 */

#include <util/delay.h>
#include "sysCore/TaskMate_public.h"
#include "drivers/i2c.h"
#include "services/lcd1602.h"

#define LCD1602_I2C_ADDR 0x7C // AiP31068L I2C address (Write mode)
#define LCD1602_CMD 0x80 // Co=1 RS = 0, Write Command
#define LCD1602_DATA 0x40 // Co=0 RS = 1, Write Data series
#define LCD1602_RAW 2
#define LCD1602_COL 16

void lcd1602(void)
{
	// lcd test
	lcd1602Init();

	lcd1602WriteString("lcd 1602 2x16");

	// do nothing after
	while( 1 ) { sysCallYieldHand(); }
}

void lcd1602SendCommand(uint8_t command)
{
	i2cCommStart(LCD1602_I2C_ADDR);
	i2cWrite(LCD1602_CMD); // Control byte: RS=0, RW=0
	i2cWrite(command);
	i2cCommStop();
	_delay_us(200); // Small delay for LCD to process the command
}

void lcd1602Init(void)
{
	_delay_ms(50); // Wait for LCD to power up

	lcd1602SendCommand(0x38); // Function Set: 8-bit mode, 2 lines, 5x8 dots
	_delay_us(50);
	lcd1602SendCommand(0x0C); // Display ON, Cursor OFF, Blink OFF
	_delay_us(50);
	lcd1602SendCommand(0x01); // Clear Display
	_delay_ms(2);
	lcd1602SendCommand(0x06); // Entry Mode: Cursor moves right, no shift
}

void lcd1602Clear(void)
{
	lcd1602SendCommand(0x01);
	_delay_ms(2);
}

void lcd1602SetCursor(uint8_t row, uint8_t col)
{
	const uint8_t row_offsets[] = {0x00, 0x40};
	lcd1602SendCommand(0x80 | (col + row_offsets[row]));
}

void lcd1602WriteString(const char *str)
{
	i2cCommStart(LCD1602_I2C_ADDR);
	i2cWrite(LCD1602_DATA);

	while( *str ) { i2cWrite(*str++); }
	i2cCommStop();
}
