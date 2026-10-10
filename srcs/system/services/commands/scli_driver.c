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
#include "tmLibc/tm_string.h"
#include "tmLibc/tm_syslog.h"

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
		if( tm_strncmp(TM_STR_RAM(argv[1U]), TM_STR_RAM(driver_cmd[i].name), TM_STRING_SIZE_MAX) ==
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

	tm_syslog(TM_STR("[driver] usage:\n"));
	tm_syslog(TM_STR("\tdriver init <driver_name>\n"));
	tm_syslog(TM_STR("\tdriver start <driver_name>\n"));
	tm_syslog(TM_STR("\tdriver stop <driver_name>\n"));
	tm_syslog(TM_STR("\tdriver list\n"));
	tm_syslog(TM_STR("\tdriver help\n"));
	return true;
}

static bool driverList(uint8_t argc, char *argv[])
{
	(void)argc;
	(void)argv;

	tm_syslog(TM_STR("[driver] drivers:\n"));
	const mod_count_t driver_count = sc_driverGetCount();
	for( mod_count_t id = 0U; id < driver_count; id++ )
	{
		const tm_string_t *name;
		tm_run_level_t run_level;
		uint8_t status_bits;
		if( !sc_driverGetInfo(id, &name, &run_level, &status_bits) ) { return false; }

		tm_syslog(TM_STR("\t%s runlevel=%i status[init=%i start=%i error=%i dead=%i]\n"),
				  name,
				  run_level,
				  TM_GETBIT(status_bits, DRV_BIT_INIT) != 0U,
				  TM_GETBIT(status_bits, DRV_BIT_START) != 0U,
				  TM_GETBIT(status_bits, DRV_BIT_ERROR) != 0U,
				  TM_GETBIT(status_bits, DRV_BIT_DEAD) != 0U);
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

	tm_string_t driver_name = TM_STR_RAM(argv[SCLIDRIVER_INDEX_NAME]);
	if( sc_driverInit(argv[SCLIDRIVER_INDEX_NAME]) )
	{
		tm_syslog(TM_STR("[driver] %s initialized\n"), &driver_name);
		return true;
	}

	tm_syslog(TM_STR("[driver] %s not initialized\n"), &driver_name);
	return false;
}

static bool driverStart(uint8_t argc, char *argv[])
{
	if( argc != SCLIDRIVER_COUNT_ACTIONARGS )
	{
		driverHelp(0U, NULL);
		return false;
	}

	tm_string_t driver_name = TM_STR_RAM(argv[SCLIDRIVER_INDEX_NAME]);
	if( sc_driverStart(argv[SCLIDRIVER_INDEX_NAME]) )
	{
		tm_syslog(TM_STR("[driver] %s started\n"), &driver_name);
		return true;
	}

	tm_syslog(TM_STR("[driver] %s not started\n"), &driver_name);
	return false;
}

static bool driverStop(uint8_t argc, char *argv[])
{
	if( argc != SCLIDRIVER_COUNT_ACTIONARGS )
	{
		driverHelp(0U, NULL);
		return false;
	}

	tm_string_t driver_name = TM_STR_RAM(argv[SCLIDRIVER_INDEX_NAME]);
	if( sc_driverStop(argv[SCLIDRIVER_INDEX_NAME]) )
	{
		tm_syslog(TM_STR("[driver] %s stopped\n"), &driver_name);
		return true;
	}

	tm_syslog(TM_STR("[driver] %s not stopped\n"), &driver_name);
	return false;
}
