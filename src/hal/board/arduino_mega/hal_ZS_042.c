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
 * @brief hal lcd implementation
 *
 */

#include "hal/board/arduino_mega/hal_ZS_042.h"

#include "hal/mcu/atmega2560/hal_i2c.h"
#include "tm_libc/tm_syslog.h"

// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

#define ZS_042_I2C_ADDR 0x68

static uint8_t buf[8];

static uint8_t bcdToBin(uint8_t bcd) { return (uint8_t)((bcd >> 4) * 10u) + (bcd & 0x0Fu); }
static uint8_t binToBcd(uint8_t val) { return (uint8_t)((val / 10u) << 4) | (val % 10u); }

void hal_ZS_042Init(void) {}
void hal_ZS_042Start(void) {}
void hal_ZS_042Stop(void) {}

uint8_t hal_ZS_042Read(hal_rtc_time_t *t)
{
	tm_syslog("i2c previous satus %x\n", TW_STATUS);

	hal_i2cCommStart(ZS_042_I2C_ADDR, I2C_WRITE);
	tm_syslog("i2c start satus %x\n", TW_STATUS);

	hal_i2cWrite(0x00); // start regsiter
	tm_syslog("i2c write satus %x\n", TW_STATUS);

	hal_i2cCommStart(ZS_042_I2C_ADDR, I2C_READ);
	tm_syslog("i2c start read satus %x\n", TW_STATUS);

	for( uint8_t i = 0; i < 6; i++ ) { hal_i2cRead(&buf[i], I2C_ACK); 	tm_syslog("i2c read i satus %x\n", TW_STATUS);
}
	hal_i2cRead(&buf[6], I2C_NACK);
	tm_syslog("i2c read satus %x\n", TW_STATUS);

	hal_i2cCommStop();
	tm_syslog("i2c stop satus %x\n", TW_STATUS);

	t->seconds = bcdToBin(buf[0] & 0x7F); // bit 7 = CH (Clock Halt), ignored
	t->minutes = bcdToBin(buf[1]);
	t->hours = bcdToBin(buf[2] & 0x3F); // 24 hour
	t->weekday = bcdToBin(buf[3]);
	t->day = bcdToBin(buf[4]);
	t->month = bcdToBin(buf[5] & 0x1F); // bit 7 = century
	t->year = bcdToBin(buf[6]);

	return true;
}

uint8_t hal_ZS_042Write(const hal_rtc_time_t *t)
{

	buf[0] = 0x00; // register start address
	buf[1] = binToBcd(t->seconds & 0x7F); // CH=0 (clock ON)
	buf[2] = binToBcd(t->minutes);
	buf[3] = binToBcd(t->hours) & 0x3F; // mode 24h
	buf[4] = binToBcd(t->weekday);
	buf[5] = binToBcd(t->day);
	buf[6] = binToBcd(t->month & 0x1F);
	buf[7] = 0x26 ; //binToBcd(t->year);

	hal_i2cCommStart(ZS_042_I2C_ADDR, I2C_WRITE);
	for( uint8_t i = 0; i < 8; i++ ) { hal_i2cWrite(buf[i]); }
	hal_i2cCommStop();

	return 0;
}
// NOLINTEND
