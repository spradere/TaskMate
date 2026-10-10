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

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <stddef.h>

#include "interfaces/drv_i2c.h"
#include "interfaces/drv_lcd.h"
#include "interfaces/hal_delay.h"
#include "interfaces/tm_macros.h"
#include "interfaces/tm_runLevel.h"

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static hal_driver_state_t lcdAMC2004Clear(void);
static hal_driver_state_t lcdAMC2004SendCommand(uint8_t command);
static hal_driver_state_t lcdSetError(err_codes_t error);

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static hal_driver_status_t lcd_status;
static err_codes_t lcd_last_error = ERR_NO_ERROR;

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define LCDAMC2004_VAL_I2C_ADDR 0x3CU // AiP31068L I2C address (Write mode)
#define LCDAMC2004_VAL_CMD 0x80U // Co=1 RS = 0, Write Command
#define LCDAMC2004_VAL_DATA 0x40U // Co=0 RS = 1, Write Data series
#define LCDAMC2004_VAL_RAW 4U
#define LCDAMC2004_VAL_COL 20U

#define LCDAMC2004_DELAY_POWERUP_ms 50U
#define LCDAMC2004_DELAY_CMDPROCESS_us 110U
#define LCDAMC2004_DELAY_CLEAR_ms 11U
#define LCDAMC2004_DELAY_I2CSTOP_us 200U

#define LCDAMC2004_CMD_SETUP 0x38U // Function Set: 8-bit mode, 2 lines, 5x8 dots
#define LCDAMC2004_CMD_DISPLAYMODE 0x0CU // Display ON, Cursor OFF, Blink OFF
#define LCDAMC2004_CMD_CLEAR 0x01U // Clear Display
#define LCDAMC2004_CMD_ENTRYMODE 0x06U // Entry Mode: Cursor moves right, no shift
#define LCDAMC2004_ADDR_ROW0 0x00U
#define LCDAMC2004_ADDR_ROW1 0x40U
#define LCDAMC2004_ADDR_ROW2 0x14U
#define LCDAMC2004_ADDR_ROW3 0x54U
#define LCDAMC2004_CMD_SETCURSOR 0x80U

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Driver life cycle
 * ---------------------------------------------*/

static hal_driver_state_t lcdSetError(err_codes_t error)
{
	lcd_last_error = error;
	return DRV_STATE_ERROR;
}
static hal_driver_state_t hal_lcdGetStatus(void)
{
	if( TM_GETBIT(lcd_status, DRV_BIT_DEAD) != 0U )
	{
		lcd_last_error = ERR_HAL_DRIVER_DEAD;
		return DRV_STATE_DEAD;
	}
	if( TM_GETBIT(lcd_status, DRV_BIT_ERROR) != 0U )
	{
		return lcdSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( hal_i2cControl(DRV_CTRL_GETSTATUS, 0U) != DRV_STATE_RUNNING )
	{
		return lcdSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}
	if( TM_GETBIT(lcd_status, DRV_BIT_INIT) == 0U )
	{
		if( TM_GETBIT(lcd_status, DRV_BIT_START) == 0U ) { return DRV_STATE_OFF; }
		return lcdSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( TM_GETBIT(lcd_status, DRV_BIT_START) == 0U ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t lcdRequireRunning(void)
{
	hal_driver_state_t state = hal_lcdGetStatus();
	if( (state == DRV_STATE_OFF) || (state == DRV_STATE_INITIALIZED) )
	{
		return lcdSetError(ERR_HAL_DRIVER_NOT_RUNNING);
	}
	return state;
}

static hal_driver_state_t hal_lcdInit(void)
{
	if( TM_GETBIT(lcd_status, DRV_BIT_DEAD) != 0U ) { return lcdSetError(ERR_HAL_DRIVER_DEAD); }
	if( hal_i2cControl(DRV_CTRL_GETSTATUS, 0U) != DRV_STATE_RUNNING )
	{
		return lcdSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}

	hal_delayMs(LCDAMC2004_DELAY_POWERUP_ms); // Wait for LCD to power up

	if( lcdAMC2004SendCommand(LCDAMC2004_CMD_SETUP) == DRV_STATE_ERROR ) { return DRV_STATE_ERROR; }
	hal_delayUs(LCDAMC2004_DELAY_CMDPROCESS_us);

	if( lcdAMC2004SendCommand(LCDAMC2004_CMD_DISPLAYMODE) == DRV_STATE_ERROR )
	{
		return DRV_STATE_ERROR;
	}
	hal_delayUs(LCDAMC2004_DELAY_CMDPROCESS_us);

	if( lcdAMC2004SendCommand(LCDAMC2004_CMD_CLEAR) == DRV_STATE_ERROR ) { return DRV_STATE_ERROR; }
	hal_delayMs(LCDAMC2004_DELAY_CLEAR_ms);

	if( lcdAMC2004SendCommand(LCDAMC2004_CMD_ENTRYMODE) == DRV_STATE_ERROR )
	{
		return DRV_STATE_ERROR;
	}
	hal_delayUs(LCDAMC2004_DELAY_CMDPROCESS_us);

	TM_SETBIT(lcd_status, DRV_BIT_INIT);
	lcd_last_error = ERR_NO_ERROR;
	return DRV_STATE_INITIALIZED;
}

static hal_driver_state_t hal_lcdStart(void)
{
	if( TM_GETBIT(lcd_status, DRV_BIT_DEAD) != 0U ) { return lcdSetError(ERR_HAL_DRIVER_DEAD); }
	if( TM_GETBIT(lcd_status, DRV_BIT_INIT) == 0U )
	{

		return lcdSetError(ERR_HAL_DRIVER_NOT_INITIALIZED);
	}
	if( lcdAMC2004Clear() == DRV_STATE_ERROR ) { return DRV_STATE_ERROR; }

	TM_SETBIT(lcd_status, DRV_BIT_START);
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t hal_lcdStop(void)
{
	// nothing to do.
	TM_CLEARBIT(lcd_status, DRV_BIT_START);
	return hal_lcdGetStatus();
}

/* -----------------------------------------------
 * LCD operations
 * ---------------------------------------------*/

static hal_driver_state_t lcdAMC2004SendCommand(uint8_t command)
{
	if( hal_i2cCommStart(LCDAMC2004_VAL_I2C_ADDR, HAL_I2C_WRITE) == DRV_STATE_ERROR )
	{
		return lcdSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}
	if( hal_i2cWrite(LCDAMC2004_VAL_CMD) == DRV_STATE_ERROR )
	{
		return lcdSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}
	if( hal_i2cWrite(command) == DRV_STATE_ERROR )
	{
		return lcdSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}
	if( hal_i2cCommStop() == DRV_STATE_ERROR ) { return lcdSetError(ERR_HAL_DRIVER_DEPENDENCY); }
	hal_delayUs(LCDAMC2004_DELAY_I2CSTOP_us); // Small delay for LCD to process the command
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t lcdAMC2004Clear(void)
{
	if( lcdAMC2004SendCommand(LCDAMC2004_CMD_CLEAR) == DRV_STATE_ERROR ) { return DRV_STATE_ERROR; }
	hal_delayMs(LCDAMC2004_DELAY_CLEAR_ms);
	return DRV_STATE_RUNNING;
}

hal_driver_state_t hal_lcdClear(void)
{
	hal_driver_state_t state = lcdRequireRunning();
	if( state != DRV_STATE_RUNNING ) { return state; }
	return lcdAMC2004Clear();
}

hal_driver_state_t hal_lcdSetCursor(uint8_t row, uint8_t col)
{
	hal_driver_state_t state = lcdRequireRunning();
	if( state != DRV_STATE_RUNNING ) { return state; }
	if( (row >= LCDAMC2004_VAL_RAW) || (col >= LCDAMC2004_VAL_COL) )
	{
		return lcdSetError(ERR_HAL_LCD_CURSOR_OUT_OF_RANGE);
	}
	const uint8_t row_offsets[] = {LCDAMC2004_ADDR_ROW0, LCDAMC2004_ADDR_ROW1,
								   LCDAMC2004_ADDR_ROW2, LCDAMC2004_ADDR_ROW3};
	return lcdAMC2004SendCommand((uint8_t)(LCDAMC2004_CMD_SETCURSOR | (col + row_offsets[row])));
}

hal_driver_state_t hal_lcdWriteStart(void)
{
	hal_driver_state_t state = lcdRequireRunning();
	if( state != DRV_STATE_RUNNING ) { return state; }

	if( hal_i2cCommStart(LCDAMC2004_VAL_I2C_ADDR, HAL_I2C_WRITE) == DRV_STATE_ERROR )
	{
		return lcdSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}
	if( hal_i2cWrite(LCDAMC2004_VAL_DATA) == DRV_STATE_ERROR )
	{
		return lcdSetError(ERR_HAL_DRIVER_DEPENDENCY);
	}
	return DRV_STATE_RUNNING;
}

hal_driver_state_t hal_lcdWriteByte(uint8_t data)
{
	if( hal_i2cWrite(data) == DRV_STATE_ERROR ) { return lcdSetError(ERR_HAL_DRIVER_DEPENDENCY); }
	return DRV_STATE_RUNNING;
}

hal_driver_state_t hal_lcdWriteEnd(void)
{
	if( hal_i2cCommStop() == DRV_STATE_ERROR ) { return lcdSetError(ERR_HAL_DRIVER_DEPENDENCY); }
	return DRV_STATE_RUNNING;
}

/* -----------------------------------------------
 * Driver control
 * ---------------------------------------------*/

hal_driver_state_t hal_lcdControl(hal_driver_control_t command, hal_driver_control_data_t *data)
{
	switch( command )
	{
		case DRV_CTRL_INIT:
			return hal_lcdInit();
		case DRV_CTRL_START:
			return hal_lcdStart();
		case DRV_CTRL_STOP:
			return hal_lcdStop();
		// Keep lifecycle flags when updating the run-level bits.
		case DRV_CTRL_RLSET:
			if( data == NULL ) { return lcdSetError(ERR_NULL_POINTER); }
			if( data->run_level >= RL_LEVEL_COUNT )
			{
				return lcdSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			lcd_status &= (hal_driver_status_t)~RL_LEVEL_MASK;
			lcd_status |= data->run_level;
			return hal_lcdGetStatus();
		case DRV_CTRL_RLGET:
			if( data == NULL ) { return lcdSetError(ERR_NULL_POINTER); }
			data->run_level = RL_GET_RUN_LEVEL(lcd_status);
			return hal_lcdGetStatus();
		// Limit bit operations to the shared driver status flags.
		case DRV_CTRL_SETBIT:
			if( data == NULL ) { return lcdSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return lcdSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			TM_SETBIT(lcd_status, data->status_bit);
			return hal_lcdGetStatus();
		case DRV_CTRL_CLEARBIT:
			if( data == NULL ) { return lcdSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return lcdSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			TM_CLEARBIT(lcd_status, data->status_bit);
			return hal_lcdGetStatus();
		case DRV_CTRL_GETBIT:
			if( data == NULL ) { return lcdSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return lcdSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			data->bit_value = TM_GETBIT(lcd_status, data->status_bit) != 0U;
			return hal_lcdGetStatus();
		// Expose current state and the most recent driver error separately.
		case DRV_CTRL_GETSTATUS:
			return hal_lcdGetStatus();
		case DRV_CTRL_GETLASTERROR:
			if( data == NULL ) { return lcdSetError(ERR_NULL_POINTER); }
			data->error = lcd_last_error;
			return hal_lcdGetStatus();
		default:

			return lcdSetError(ERR_HAL_DRIVER_INVALID_CONTROL);
	}
}
