/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file rtc_ZS042.c
 * @brief rtc zs042 implementation.
 *
 */

#include "rtc_ZS042.h"

#include "hal/public/i2c.h"
#include "interfaces/macros.h"
#include "interfaces/modules_define.h"
#include "interfaces/runLevel_define.h"

// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

#define ZS042_I2C_ADDR 0x68

static uint8_t buf[7];
static hal_driver_status_t rtc_status;

static uint8_t bcdToBin(uint8_t bcd) { return (uint8_t)((bcd >> 4) * 10u) + (bcd & 0x0Fu); }
static uint8_t binToBcd(uint8_t val) { return (uint8_t)((val / 10u) << 4) | (val % 10u); }

static uint8_t hal_rtcGetStatus(void)
{
	if( TM_GETBIT(rtc_status, DRV_BIT_ERROR) != 0 ) { return DRV_STATE_ERROR; }
	if( hal_i2cControl(DRV_CTRL_GETSTATUS, 0) != DRV_STATE_RUNNING ) { return DRV_STATE_ERROR; }
	if( TM_GETBIT(rtc_status, DRV_BIT_INIT) == 0 )
	{
		if( TM_GETBIT(rtc_status, DRV_BIT_START) == 0 ) { return DRV_STATE_OFF; }
		return DRV_STATE_ERROR;
	}
	if( TM_GETBIT(rtc_status, DRV_BIT_START) == 0 ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

static uint8_t hal_rtcInit(void)
{
	hal_rtcControl(DRV_CTRL_SETBIT, DRV_BIT_INIT);
	return 0;
}

static uint8_t hal_rtcStart(void)
{
	if( (hal_rtcControl(DRV_CTRL_GETBIT, DRV_BIT_INIT) == 0) ||
		(hal_rtcControl(DRV_CTRL_GETBIT, DRV_BIT_DEAD) != 0) )
	{
		return DRV_UNKNOW;
	}

	hal_rtcControl(DRV_CTRL_SETBIT, DRV_BIT_START);
	return 0;
}

static uint8_t hal_rtcStop(void)
{
	hal_rtcControl(DRV_CTRL_CLEARBIT, DRV_BIT_START);
	return 0;
}

uint8_t hal_rtcRead(hal_rtc_time_t *t)
{
	if( hal_rtcGetStatus() != DRV_STATE_RUNNING ) { return DRV_STATE_ERROR; }
	hal_i2cCommStart(ZS042_I2C_ADDR, HAL_I2C_WRITE);
	hal_i2cWrite(0x00); // start register

	hal_i2cCommStart(ZS042_I2C_ADDR, HAL_I2C_READ);

	for( uint8_t i = 0; i < 6; i++ ) { hal_i2cRead(&buf[i], HAL_I2C_ACK); }
	hal_i2cRead(&buf[6], HAL_I2C_NACK);

	hal_i2cCommStop();

	t->seconds = bcdToBin(buf[0] & 0x7F); // bit 7 = Clock Halt, ignored
	t->minutes = bcdToBin(buf[1]);
	t->hours = bcdToBin(buf[2] & 0x3F); // 24 hour
	t->weekday = bcdToBin(buf[3]);
	t->day = bcdToBin(buf[4]);
	t->month = bcdToBin(buf[5] & 0x1F); // bit 7 = century
	t->year = bcdToBin(buf[6]);

	return true;
}

uint8_t hal_rtcWrite(const hal_rtc_time_t *t)
{
	if( hal_rtcGetStatus() != DRV_STATE_RUNNING ) { return DRV_STATE_ERROR; }
	buf[0] = binToBcd(t->seconds & 0x7F); // bit 7 = 0, clock ON
	buf[1] = binToBcd(t->minutes);
	buf[2] = binToBcd(t->hours) & 0x3F; // mode 24h
	buf[3] = binToBcd(t->weekday);
	buf[4] = binToBcd(t->day);
	buf[5] = binToBcd(t->month & 0x1F);
	buf[6] = binToBcd(t->year);

	hal_i2cCommStart(ZS042_I2C_ADDR, HAL_I2C_WRITE);
	hal_i2cWrite(0x00); // register start address
	for( uint8_t i = 0; i < 7; i++ ) { hal_i2cWrite(buf[i]); }
	hal_i2cCommStop();

	return 0;
}

uint8_t hal_rtcControl(uint8_t cmd, uint8_t val)
{
	switch( cmd )
	{
		case DRV_CTRL_INIT:
			return hal_rtcInit();
		case DRV_CTRL_START:
			return hal_rtcStart();
		case DRV_CTRL_STOP:
			return hal_rtcStop();
		case DRV_CTRL_RLSET:
			rtc_status &= (hal_driver_status_t)~RL_LEVEL_MASK;
			rtc_status |= val;
			return 0;
		case DRV_CTRL_RLGET:
			return rtc_status & RL_LEVEL_MASK;
		case DRV_CTRL_SETBIT:
			TM_SETBIT(rtc_status, val);
			return 0;
		case DRV_CTRL_CLEARBIT:
			TM_CLEARBIT(rtc_status, val);
			return 0;
		case DRV_CTRL_GETBIT:
			return TM_GETBIT(rtc_status, val);
		case DRV_CTRL_GETSTATUS:
			return hal_rtcGetStatus();
		default:
			return DRV_UNKNOW;
	}
}

// NOLINTEND
