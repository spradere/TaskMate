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

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <stddef.h>

#include "interfaces/drv_i2c.h"
#include "interfaces/drv_rtc.h"
#include "interfaces/tm_macros.h"
#include "interfaces/tm_runLevel.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define RTCZS042_ADDR_I2C 0x68U
#define RTCZS042_COUNT_REGISTER 7U
#define RTCZS042_SHIFT_BCD 4U
#define RTCZS042_BASE_DECIMAL 10U
#define RTCZS042_MASK_BCD 0x0FU
#define RTCZS042_MASK_SECONDS 0x7FU
#define RTCZS042_MASK_HOURS 0x3FU
#define RTCZS042_MASK_MONTH 0x1FU
#define RTCZS042_INDEX_SECONDS 0U
#define RTCZS042_INDEX_MINUTES 1U
#define RTCZS042_INDEX_HOURS 2U
#define RTCZS042_INDEX_WEEKDAY 3U
#define RTCZS042_INDEX_DAY 4U
#define RTCZS042_INDEX_MONTH 5U
#define RTCZS042_INDEX_YEAR 6U
#define RTCZS042_MAX_SECONDS 59U
#define RTCZS042_MAX_MINUTES 59U
#define RTCZS042_MAX_HOURS 23U
#define RTCZS042_MAX_WEEKDAY 7U
#define RTCZS042_MAX_DAY 31U
#define RTCZS042_MAX_MONTH 12U
#define RTCZS042_MAX_YEAR 99U

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static uint8_t buf[RTCZS042_COUNT_REGISTER];
static hal_driver_status_t rtc_status;
static err_codes_t rtc_last_error = ERR_NO_ERROR;

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Time conversion
 * ---------------------------------------------*/

static uint8_t bcdToBin(uint8_t bcd)
{
	return (uint8_t)((bcd >> RTCZS042_SHIFT_BCD) * RTCZS042_BASE_DECIMAL) +
		   (bcd & RTCZS042_MASK_BCD);
}
static uint8_t binToBcd(uint8_t val)
{
	return (uint8_t)((val / RTCZS042_BASE_DECIMAL) << RTCZS042_SHIFT_BCD) |
		   (val % RTCZS042_BASE_DECIMAL);
}

/* -----------------------------------------------
 * Driver life cycle
 * ---------------------------------------------*/

static hal_driver_state_t rtcSetError(err_codes_t error)
{
	rtc_last_error = error;
	return DRV_STATE_ERROR;
}
static hal_driver_state_t hal_rtcGetStatus(void)
{
	if( TM_GETBIT(rtc_status, DRV_BIT_DEAD) != 0U )
	{
		rtc_last_error = ERR_HAL_DRIVER_DEAD;
		return DRV_STATE_DEAD;
	}
	if( TM_GETBIT(rtc_status, DRV_BIT_ERROR) != 0U )
	{
		return rtcSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( hal_i2cControl(DRV_CTRL_GETSTATUS, 0U) != DRV_STATE_RUNNING )
	{
		return rtcSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}
	if( TM_GETBIT(rtc_status, DRV_BIT_INIT) == 0U )
	{
		if( TM_GETBIT(rtc_status, DRV_BIT_START) == 0U ) { return DRV_STATE_OFF; }
		return rtcSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( TM_GETBIT(rtc_status, DRV_BIT_START) == 0U ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t rtcRequireRunning(void)
{
	hal_driver_state_t state = hal_rtcGetStatus();
	if( (state == DRV_STATE_OFF) || (state == DRV_STATE_INITIALIZED) )
	{
		return rtcSetError(ERR_HAL_DRIVER_NOT_RUNNING);
	}
	return state;
}

static hal_driver_state_t hal_rtcInit(void)
{
	if( TM_GETBIT(rtc_status, DRV_BIT_DEAD) != 0U ) { return rtcSetError(ERR_HAL_DRIVER_DEAD); }
	if( hal_i2cControl(DRV_CTRL_GETSTATUS, 0U) != DRV_STATE_RUNNING )
	{

		return rtcSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}
	TM_SETBIT(rtc_status, DRV_BIT_INIT);
	rtc_last_error = ERR_NO_ERROR;
	return DRV_STATE_INITIALIZED;
}

static hal_driver_state_t hal_rtcStart(void)
{
	if( TM_GETBIT(rtc_status, DRV_BIT_DEAD) != 0U ) { return rtcSetError(ERR_HAL_DRIVER_DEAD); }
	if( TM_GETBIT(rtc_status, DRV_BIT_INIT) == 0U )
	{
		return rtcSetError(ERR_HAL_DRIVER_NOT_INITIALIZED);
	}

	TM_SETBIT(rtc_status, DRV_BIT_START);
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t hal_rtcStop(void)
{
	TM_CLEARBIT(rtc_status, DRV_BIT_START);
	return hal_rtcGetStatus();
}

/* -----------------------------------------------
 * RTC operations
 * ---------------------------------------------*/

hal_driver_state_t hal_rtcRead(hal_rtc_time_t *time)
{
	hal_driver_state_t state = rtcRequireRunning();
	if( state != DRV_STATE_RUNNING ) { return state; }
	if( time == NULL ) { return rtcSetError(ERR_NULL_POINTER); }
	if( hal_i2cCommStart(RTCZS042_ADDR_I2C, HAL_I2C_WRITE) == DRV_STATE_ERROR )
	{
		return rtcSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}
	if( hal_i2cWrite(RTCZS042_INDEX_SECONDS) == DRV_STATE_ERROR )
	{
		return rtcSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}

	if( hal_i2cCommStart(RTCZS042_ADDR_I2C, HAL_I2C_READ) == DRV_STATE_ERROR )
	{
		return rtcSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}

	for( uint8_t i = 0U; i < RTCZS042_INDEX_YEAR; i++ )
	{
		if( hal_i2cRead(&buf[i], HAL_I2C_ACK) == DRV_STATE_ERROR )
		{
			return rtcSetError(ERR_HAL_DRIVER_DEPENDENCY);
		}
	}
	if( hal_i2cRead(&buf[RTCZS042_INDEX_YEAR], HAL_I2C_NACK) == DRV_STATE_ERROR )
	{
		return rtcSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}

	if( hal_i2cCommStop() == DRV_STATE_ERROR ) { return rtcSetError(ERR_HAL_DRIVER_DEPENDENCY); }

	time->seconds = bcdToBin(buf[RTCZS042_INDEX_SECONDS] & RTCZS042_MASK_SECONDS);
	time->minutes = bcdToBin(buf[RTCZS042_INDEX_MINUTES]);
	time->hours = bcdToBin(buf[RTCZS042_INDEX_HOURS] & RTCZS042_MASK_HOURS);
	time->weekday = bcdToBin(buf[RTCZS042_INDEX_WEEKDAY]);
	time->day = bcdToBin(buf[RTCZS042_INDEX_DAY]);
	time->month = bcdToBin(buf[RTCZS042_INDEX_MONTH] & RTCZS042_MASK_MONTH);
	time->year = bcdToBin(buf[RTCZS042_INDEX_YEAR]);

	return DRV_STATE_RUNNING;
}

hal_driver_state_t hal_rtcWrite(const hal_rtc_time_t *time)
{
	hal_driver_state_t state = rtcRequireRunning();
	if( state != DRV_STATE_RUNNING ) { return state; }
	if( time == NULL ) { return rtcSetError(ERR_NULL_POINTER); }
	// Reject invalid calendar fields before writing any device register.
	if( (time->seconds > RTCZS042_MAX_SECONDS) || (time->minutes > RTCZS042_MAX_MINUTES) ||
		(time->hours > RTCZS042_MAX_HOURS) || (time->weekday < 1U) ||
		(time->weekday > RTCZS042_MAX_WEEKDAY) || (time->day < 1U) ||
		(time->day > RTCZS042_MAX_DAY) || (time->month < 1U) ||
		(time->month > RTCZS042_MAX_MONTH) || (time->year > RTCZS042_MAX_YEAR) )
	{
		return rtcSetError(ERR_HAL_RTC_TIME_OUT_OF_RANGE);
	}
	// Encode the seven RTC registers in the device's BCD format.
	buf[RTCZS042_INDEX_SECONDS] = binToBcd(time->seconds & RTCZS042_MASK_SECONDS);
	buf[RTCZS042_INDEX_MINUTES] = binToBcd(time->minutes);
	buf[RTCZS042_INDEX_HOURS] = binToBcd(time->hours) & RTCZS042_MASK_HOURS;
	buf[RTCZS042_INDEX_WEEKDAY] = binToBcd(time->weekday);
	buf[RTCZS042_INDEX_DAY] = binToBcd(time->day);
	buf[RTCZS042_INDEX_MONTH] = binToBcd(time->month & RTCZS042_MASK_MONTH);
	buf[RTCZS042_INDEX_YEAR] = binToBcd(time->year);

	// Select register zero, then write the complete time record in order.
	if( hal_i2cCommStart(RTCZS042_ADDR_I2C, HAL_I2C_WRITE) == DRV_STATE_ERROR )
	{
		return rtcSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}
	if( hal_i2cWrite(RTCZS042_INDEX_SECONDS) == DRV_STATE_ERROR )
	{
		return rtcSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}
	for( uint8_t i = 0U; i < RTCZS042_COUNT_REGISTER; i++ )
	{
		if( hal_i2cWrite(buf[i]) == DRV_STATE_ERROR )
		{
			return rtcSetError(ERR_HAL_DRIVER_DEPENDENCY);
		}
	}
	if( hal_i2cCommStop() == DRV_STATE_ERROR ) { return rtcSetError(ERR_HAL_DRIVER_DEPENDENCY); }

	return DRV_STATE_RUNNING;
}

/* -----------------------------------------------
 * Driver control
 * ---------------------------------------------*/

hal_driver_state_t hal_rtcControl(hal_driver_control_t command, hal_driver_control_data_t *data)
{
	switch( command )
	{
		case DRV_CTRL_INIT:
			return hal_rtcInit();
		case DRV_CTRL_START:
			return hal_rtcStart();
		case DRV_CTRL_STOP:
			return hal_rtcStop();
		// Keep lifecycle flags when updating the run-level bits.
		case DRV_CTRL_RLSET:
			if( data == NULL ) { return rtcSetError(ERR_NULL_POINTER); }
			if( data->run_level >= RL_LEVEL_COUNT )
			{
				return rtcSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			rtc_status &= (hal_driver_status_t)~RL_LEVEL_MASK;
			rtc_status |= data->run_level;
			return hal_rtcGetStatus();
		case DRV_CTRL_RLGET:
			if( data == NULL ) { return rtcSetError(ERR_NULL_POINTER); }
			data->run_level = rtc_status & RL_LEVEL_MASK;
			return hal_rtcGetStatus();
		// Limit bit operations to the shared driver status flags.
		case DRV_CTRL_SETBIT:
			if( data == NULL ) { return rtcSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return rtcSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			TM_SETBIT(rtc_status, data->status_bit);
			return hal_rtcGetStatus();
		case DRV_CTRL_CLEARBIT:
			if( data == NULL ) { return rtcSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return rtcSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			TM_CLEARBIT(rtc_status, data->status_bit);
			return hal_rtcGetStatus();
		case DRV_CTRL_GETBIT:
			if( data == NULL ) { return rtcSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return rtcSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			data->bit_value = TM_GETBIT(rtc_status, data->status_bit) != 0U;
			return hal_rtcGetStatus();
		// Expose current state and the most recent driver error separately.
		case DRV_CTRL_GETSTATUS:
			return hal_rtcGetStatus();
		case DRV_CTRL_GETLASTERROR:
			if( data == NULL ) { return rtcSetError(ERR_NULL_POINTER); }
			data->error = rtc_last_error;
			return hal_rtcGetStatus();
		default:

			return rtcSetError(ERR_HAL_DRIVER_INVALID_CONTROL);
	}
}
