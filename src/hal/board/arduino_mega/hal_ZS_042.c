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
 * @file hal_ZS_042.c
 * @brief hal lcd implemetation
 *
 */

#include "hal/board/arduino_mega/hal_ZS_042.h"
#include "hal/mcu/atmega2560/hal_i2c.h"

#define ZS_042_I2C_ADDR 0x68

static uint8_t bcdToUint8(uint8_t bcd) { return (uint8_t)((bcd >> 4) * 10u) + (bcd & 0x0Fu); }

static uint8_t uint8ToBcd(uint8_t val) { return (uint8_t)((val / 10u) << 4) | (val % 10u); }

void hal_ZS_042Init(void) {}
void hal_ZS_042Start(void) {}
void hal_ZS_042Stop(void) {}

uint8_t hal_ZS_042Read(hal_rtc_time_t *t)
{
	uint8_t buf[7];

	hal_i2cCommStart(ZS_042_I2C_ADDR);
	hal_i2cWrite(0); // command ???

	for( uint8_t i = 0; i < 6; i++ ) { buf[i] = hal_i2cRead(true); }
	buf[7] = hal_i2cRead(false);
	hal_i2cCommStop();

	t->seconds = bcdToUint8(buf[0] & 0x7F); // bit 7 = CH (Clock Halt), ignored
	t->minutes = bcdToUint8(buf[1]);
	t->hours = bcdToUint8(buf[2] & 0x3F); // 24 hour
	t->weekday = bcdToUint8(buf[3]);
	t->day = bcdToUint8(buf[4]);
	t->month = bcdToUint8(buf[5] & 0x1F); // bit 7 = century
	t->year = bcdToUint8(buf[6]);

	return true;
}

uint8_t hal_ZS_042Write(const hal_rtc_time_t *t)
{
	uint8_t buf[8];

	buf[0] = 0x00; // adresse registre de départ
	buf[1] = uint8ToBcd(t->seconds & 0x7F); // CH=0 (clock ON)
	buf[2] = uint8ToBcd(t->minutes);
	buf[3] = uint8ToBcd(t->hours) & 0x3F; // forcer mode 24h
	buf[4] = uint8ToBcd(t->weekday);
	buf[5] = uint8ToBcd(t->day);
	buf[6] = uint8ToBcd(t->month & 0x1F);
	buf[7] = uint8ToBcd(t->year);

	hal_i2cCommStart(ZS_042_I2C_ADDR);
	for( uint8_t i = 0; i < 8; i++ ) { hal_i2cWrite(buf[i]); }
	hal_i2cCommStop();

	return 0;
}
