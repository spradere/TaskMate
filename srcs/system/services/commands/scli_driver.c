/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file scli_driver.c
 * @brief Driver command implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "scli_driver.h"

#include "interfaces/hal_drivers.h"
#include "interfaces/tm_define.h"
#include "interfaces/tm_macros.h"
#include "system/sysCall/sc_driver.h"
#include "system/sysCall/sc_errors.h"
#include "system/sysCall/sc_string.h"
#include "tmLibc/tm_stdio.h"
#include "tmLibc/tm_string.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define SCLIDRIVER_COUNT_COMMANDARGS 2U
#define SCLIDRIVER_COUNT_ACTIONARGS 3U
#define SCLIDRIVER_INDEX_NAME 2U

/* -----------------------------------------------
 * Private types
 * ---------------------------------------------*/

typedef bool (*driver_cmd_func_t)(uint8_t argc, char *argv[]);

typedef struct
{
	const char *name;
	driver_cmd_func_t func;
} driver_cmd_t;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static bool driverInit(uint8_t argc, char *argv[]);
static bool driverStart(uint8_t argc, char *argv[]);
static bool driverStop(uint8_t argc, char *argv[]);
static bool driverList(uint8_t argc, char *argv[]);
static bool driverHelp(uint8_t argc, char *argv[]);

/* -----------------------------------------------
 * Command table
 * ---------------------------------------------*/

static const driver_cmd_t driver_cmd[] = {
	{"init", driverInit},
	{"start", driverStart},
	{"stop", driverStop},
	{"list", driverList},
	{"help", driverHelp},
	{NULL, NULL},
};

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Command dispatch
 * ---------------------------------------------*/

bool driverCommand(uint8_t argc, char *argv[])
{
	if( argc < SCLIDRIVER_COUNT_COMMANDARGS )
	{
		driverHelp(0U, NULL);
		return false;
	}

	for( uint8_t i = 0U; driver_cmd[i].name != NULL; i++ )
	{
		if( strncmp(TM_STR_RAM(argv[1U]), TM_STR_RAM(driver_cmd[i].name), TM_STRING_SIZE_MAX) ==
			0U )
		{
			return driver_cmd[i].func(argc, argv);
		}
	}

	driverHelp(0U, NULL);
	return false;
}

/* -----------------------------------------------
 * Command handlers
 * ---------------------------------------------*/

static bool driverHelp(uint8_t argc, char *argv[])
{
	(void)argc;
	(void)argv;

	printf(TM_STR("[driver] usage:\n"));
	printf(TM_STR("\tdriver init <driver_name>\n"));
	printf(TM_STR("\tdriver start <driver_name>\n"));
	printf(TM_STR("\tdriver stop <driver_name>\n"));
	printf(TM_STR("\tdriver list\n"));
	printf(TM_STR("\tdriver help\n"));
	return true;
}

static bool driverList(uint8_t argc, char *argv[])
{
	(void)argc;
	(void)argv;

	printf(TM_STR("[driver] drivers:\n"));
	const mod_count_t driver_count = sc_driverGetCount();
	for( mod_count_t id = 0U; id < driver_count; id++ )
	{
		const tm_string_t *name;
		tm_run_level_t run_level;
		hal_driver_status_t status_bits;
		if( !sc_driverGetInfo(id, &name, &run_level, &status_bits) ) { return false; }

		printf(TM_STR("\t%s runlevel=%u status[init=%u start=%u error=%u dead=%u]\n"),
				  name,
				  (unsigned int)run_level,
				  (unsigned int)(TM_GETBIT(status_bits, DRV_BIT_INIT) != 0U),
				  (unsigned int)(TM_GETBIT(status_bits, DRV_BIT_START) != 0U),
				  (unsigned int)(TM_GETBIT(status_bits, DRV_BIT_ERROR) != 0U),
				  (unsigned int)(TM_GETBIT(status_bits, DRV_BIT_DEAD) != 0U));
	}

	return true;
}

static bool driverInit(uint8_t argc, char *argv[])
{
	if( argc != SCLIDRIVER_COUNT_ACTIONARGS )
	{
		driverHelp(0U, NULL);
		return false;
	}

	tm_string_arg_t driver_name = TM_STR_RAM(argv[SCLIDRIVER_INDEX_NAME]);
	err_code_t error = sc_driverInit(argv[SCLIDRIVER_INDEX_NAME]);
	if( error == ERR_NO_ERROR )
	{
		printf(TM_STR("[driver] %s initialized\n"), TM_STR_ARG(driver_name));
		return true;
	}

	const tm_string_t *message = err_getMessage((err_codes_t)error);
	if( message != NULL )
	{
		printf(TM_STR("[driver] %s not initialized: %s\n"), TM_STR_ARG(driver_name), message);
	}
	else { printf(TM_STR("[driver] %s not initialized: error %u\n"), TM_STR_ARG(driver_name),
					 (unsigned int)error); }
	return false;
}

static bool driverStart(uint8_t argc, char *argv[])
{
	if( argc != SCLIDRIVER_COUNT_ACTIONARGS )
	{
		driverHelp(0U, NULL);
		return false;
	}

	tm_string_arg_t driver_name = TM_STR_RAM(argv[SCLIDRIVER_INDEX_NAME]);
	err_code_t error = sc_driverStart(argv[SCLIDRIVER_INDEX_NAME]);
	if( error == ERR_NO_ERROR )
	{
		printf(TM_STR("[driver] %s started\n"), TM_STR_ARG(driver_name));
		return true;
	}

	const tm_string_t *message = err_getMessage((err_codes_t)error);
	if( message != NULL )
	{
		printf(TM_STR("[driver] %s not started: %s\n"), TM_STR_ARG(driver_name), message);
	}
	else { printf(TM_STR("[driver] %s not started: error %u\n"), TM_STR_ARG(driver_name),
					 (unsigned int)error); }
	return false;
}

static bool driverStop(uint8_t argc, char *argv[])
{
	if( argc != SCLIDRIVER_COUNT_ACTIONARGS )
	{
		driverHelp(0U, NULL);
		return false;
	}

	tm_string_arg_t driver_name = TM_STR_RAM(argv[SCLIDRIVER_INDEX_NAME]);
	err_code_t error = sc_driverStop(argv[SCLIDRIVER_INDEX_NAME]);
	if( error == ERR_NO_ERROR )
	{
		printf(TM_STR("[driver] %s stopped\n"), TM_STR_ARG(driver_name));
		return true;
	}

	const tm_string_t *message = err_getMessage((err_codes_t)error);
	if( message != NULL )
	{
		printf(TM_STR("[driver] %s not stopped: %s\n"), TM_STR_ARG(driver_name), message);
	}
	else { printf(TM_STR("[driver] %s not stopped: error %u\n"), TM_STR_ARG(driver_name),
					 (unsigned int)error); }
	return false;
}
