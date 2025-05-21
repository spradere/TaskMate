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
 * @file lcdAMC2004.c
 * @brief implementation of lcd display AMC2004 4x20
 *
 * @todo Nothing
 */

#include <util/delay.h>
#include "sysCore/TaskMate_public.h"
#include "drivers/i2c.h"
#include "services/lcdAMC2004.h"

#define LCDAMC2004_I2C_ADDR 0x78 // AiP31068L I2C address (Write mode)
#define LCDAMC2004_CMD 0x80 // Co=1 RS = 0, Write Command
#define LCDAMC2004_DATA 0x40 // Co=0 RS = 1, Write Data series
#define LCDAMC2004_RAW 4
#define LCDAMC2004_COL 20

void lcdAMC2004(void)
{
	// lcd test
	//lcdAMC2004Init();
	//lcdAMC2004WriteString("lcd AMC2004");

	/*lcdAMC2004SetCursor(1,0);
	lcdAMC2004WriteString("Test msg server");

	lcdAMC2004SetCursor(2,0);
	lcdAMC2004WriteString("11/05/2025 22:45");

	lcdAMC2004SetCursor(3,10);
	lcdAMC2004WriteString("No error");*/


	// do nothing after

	while( 1 ) { sysCallYieldHand(); }
}

void lcdAMC2004SendCommand(uint8_t command)
{
	i2cCommStart(LCDAMC2004_I2C_ADDR);
	i2cWrite(LCDAMC2004_CMD); // Control byte: RS=0, RW=0
	i2cWrite(command);
	i2cCommStop();
	_delay_us(200); // Small delay for LCD to process the command
}

void lcdAMC2004Init(void)
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

void lcdAMC2004Clear(void)
{
	lcdAMC2004SendCommand(0x01);
	_delay_ms(2);
}

void lcdAMC2004SetCursor(uint8_t row, uint8_t col)
{
	const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	lcdAMC2004SendCommand(0x80 | (col + row_offsets[row]));
}

void lcdAMC2004WriteString(const char *str)
{
	i2cCommStart(LCDAMC2004_I2C_ADDR);
	i2cWrite(LCDAMC2004_DATA);

	while( *str ) { i2cWrite(*str++); }
	i2cCommStop();
}
