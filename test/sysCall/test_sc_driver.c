/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_sc_driver.c
 * @brief Exercise driver lifecycle errors and status metadata.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "interfaces/tm_string.h"

#define HAL_STRING(string) ((tm_string_t){.text = (const uint8_t *)(string), .storage = TM_MEM_ROM})
#define HAL_STRING_RAM(string) \
	((tm_string_t){.text = (const uint8_t *)(string), .storage = TM_MEM_RAM})
#define HAL_STRING_ROM(string) HAL_STRING(string)
#define HAL_STRING_INROM(name, txt) \
	static const tm_string_t name = {.text = (const uint8_t *)(txt), .storage = TM_MEM_ROM}

#include "system/sysCall/sc_driver.c"
#include "system/sysCall/sc_errors.c"

/* -----------------------------------------------
 * Test state
 * ---------------------------------------------*/

static mod_driver_item_t test_drivers[MOD_DRIVER_COUNT];
static hal_driver_state_t test_state;
static err_codes_t test_error;
static hal_driver_status_t test_status;
static hal_driver_control_t test_command;
static uint8_t test_error_reads;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static hal_driver_state_t testControl(hal_driver_control_t command,
									  hal_driver_control_data_t *data);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Syscall dependencies
 * ---------------------------------------------*/

mod_driver_item_t *mod_driverGetPointer(mod_count_t id) { return &test_drivers[id]; }

int sc_stringCompare(tm_string_t left, tm_string_t right, uint8_t size)
{
	return strncmp((const char *)left.text, (const char *)right.text, size);
}

static hal_driver_state_t testControl(hal_driver_control_t command,
									  hal_driver_control_data_t *data)
{
	if( command == DRV_CTRL_GETLASTERROR )
	{
		test_error_reads++;
		data->error = test_error;
		return test_state;
	}
	if( command == DRV_CTRL_RLGET )
	{
		data->run_level = RL_RUN_DRIVER;
		return DRV_STATE_OFF;
	}
	if( command == DRV_CTRL_GETBIT )
	{
		data->bit_value = TM_GETBIT(test_status, data->status_bit) != 0U;
		return DRV_STATE_OFF;
	}
	test_command = command;
	return test_state;
}

/* -----------------------------------------------
 * Test cases
 * ---------------------------------------------*/

int main(void)
{
	for( mod_count_t id = 0U; id < MOD_DRIVER_COUNT; id++ )
	{
		test_drivers[id].control = testControl;
	}

	test_state = DRV_STATE_INITIALIZED;
	assert(sc_driverInit("timerContext") == ERR_NO_ERROR);
	assert(test_command == DRV_CTRL_INIT);
	assert(test_error_reads == 0U);

	test_state = DRV_STATE_RUNNING;
	assert(sc_driverStart("timerContext") == ERR_NO_ERROR);
	assert(test_command == DRV_CTRL_START);

	test_state = DRV_STATE_OFF;
	assert(sc_driverStop("timerContext") == ERR_NO_ERROR);
	assert(test_command == DRV_CTRL_STOP);
	assert(test_error_reads == 0U);

	test_state = DRV_STATE_ERROR;
	test_error = ERR_HAL_DRIVER_DEPENDENCY;
	assert(sc_driverInit("timerContext") == ERR_HAL_DRIVER_DEPENDENCY);
	assert(test_error_reads == 1U);

	test_state = DRV_STATE_DEAD;
	test_error = ERR_HAL_DRIVER_DEAD;
	assert(sc_driverStart("timerContext") == ERR_HAL_DRIVER_DEAD);
	assert(test_error_reads == 2U);

	test_error = ERR_NO_ERROR;
	assert(sc_driverStop("timerContext") == ERR_HAL_DRIVER_DEAD);
	test_state = DRV_STATE_ERROR;
	assert(sc_driverStop("timerContext") == ERR_HAL_DRIVER_INVALID_STATE);
	assert(test_error_reads == 4U);

	assert(sc_driverStop("missing") == ERR_DRIVER_NOT_FOUND);
	assert(sc_driverInit(NULL) == ERR_NULL_POINTER);
	assert(test_error_reads == 4U);
	assert(strcmp((const char *)err_getMessage(ERR_DRIVER_NOT_FOUND)->text,
				  "Driver name was not found") == 0);

	const tm_string_t *name;
	tm_run_level_t run_level;
	hal_driver_status_t status_bits;
	test_status = 0U;
	TM_SETBIT(test_status, DRV_BIT_INIT);
	TM_SETBIT(test_status, DRV_BIT_DEAD);
	assert(sc_driverGetInfo(0U, &name, &run_level, &status_bits));
	assert(strcmp((const char *)name->text, "timerContext") == 0);
	assert(run_level == RL_RUN_DRIVER);
	assert(status_bits == test_status);
	assert(!sc_driverGetInfo(MOD_DRIVER_COUNT, &name, &run_level, &status_bits));
	assert(!sc_driverGetInfo(0U, &name, &run_level, NULL));

	return 0;
}
