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
 * @file lcd.c
 * @brief implementation of lcd display
 *
 * @todo Nothing
 */

#include <util/delay.h>
#include "sysCore/TaskMate_public.h"
#include "drivers/i2c.h"
#include "tasks/lcd.h"

#define LCD_I2C_ADDR 0x7C // AiP31068L I2C address (Write mode)
#define LCD_CMD 0x80 // Co=1 RS = 0, Write Command
#define LCD_DATA 0x40 // Co=0 RS = 1, Write Data series


// status
uint8_t lcd_status = 0;

void lcdSetStatus(uint8_t status) { lcd_status = status; }
uint8_t lcdGetStatus(void) { return lcd_status; }


void lcd(void)
{
	// lcd test
	lcdInit();

	lcdWriteString("status reading ...");

	// must use sysCallYield(), but not implemented !
	// do nothing there.
	while (1);

	return;
}

void lcdSendCommand(uint8_t command)
{
	i2cCommStart(LCD_I2C_ADDR);
	i2cWrite(LCD_CMD); // Control byte: RS=0, RW=0
	i2cWrite(command);
	i2cCommStop();
	_delay_us(200); // Small delay for LCD to process the command
}

void lcdSendData(uint8_t data)
{
	/*i2cCommStart(LCD_I2C_ADDR);
	i2cWrite(LCD_DATA); // Control byte: RS=1
	i2cWrite(data);
	i2cCommStop();*/
}

void lcdInit(void)
{
	_delay_ms(50); // Wait for LCD to power up

	lcdSendCommand(0x38); // Function Set: 8-bit mode, 2 lines, 5x8 dots
	_delay_us(50);
	lcdSendCommand(0x0C); // Display ON, Cursor OFF, Blink OFF
	_delay_us(50);
	lcdSendCommand(0x01); // Clear Display
	_delay_ms(2);
	lcdSendCommand(0x06); // Entry Mode: Cursor moves right, no shift
}

void lcdClear(void)
{
	lcdSendCommand(0x01);
	_delay_ms(2);
}

void lcdSetCursor(uint8_t row, uint8_t col)
{
	uint8_t row_offsets[] = {0x00, 0x40};
	lcdSendCommand(0x80 | (col + row_offsets[row]));
}

void lcdWriteString(const char *str)
{
	i2cCommStart(LCD_I2C_ADDR);
	i2cWrite(LCD_DATA);

	while (*str)
	{
		i2cWrite(*str++);
	}
	i2cCommStop();
}
