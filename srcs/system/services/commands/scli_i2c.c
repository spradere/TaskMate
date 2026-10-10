/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file scli_i2c.c
 * @brief I2C command implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "scli_i2c.h"

#include "interfaces/tm_define.h"
#include "system/sysCall/sc_driver.h"
#include "system/sysCall/sc_errors.h"
#include "tmLibc/tm_string.h"
#include "tmLibc/tm_syslog.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define SCLII2C_COUNT_COMMANDARGS 2U

/* -----------------------------------------------
 * Private types
 * ---------------------------------------------*/

typedef bool (*i2c_cmd_func_t)(uint8_t argc, char *argv[]);

typedef struct
{
	const char *name;
	i2c_cmd_func_t func;
} i2c_cmd_t;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static bool i2cScan(uint8_t argc, char *argv[]);
static bool i2cHelp(uint8_t argc, char *argv[]);

/* -----------------------------------------------
 * Command table
 * ---------------------------------------------*/

static const i2c_cmd_t i2c_cmd[] = {
	{"scan", i2cScan},
	{"help", i2cHelp},
	{NULL, NULL},
};

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Command dispatch
 * ---------------------------------------------*/

bool i2cCommand(uint8_t argc, char *argv[])
{
	if( argc < SCLII2C_COUNT_COMMANDARGS )
	{
		i2cHelp(0U, NULL);
		return false;
	}

	for( uint8_t i = 0U; i2c_cmd[i].name != NULL; i++ )
	{
		if( tm_strncmp(TM_STR_RAM(argv[1U]), TM_STR_RAM(i2c_cmd[i].name), TM_STRING_SIZE_MAX) ==
			0U )
		{
			return i2c_cmd[i].func(argc, argv);
		}
	}

	i2cHelp(0U, NULL);
	return false;
}

/* -----------------------------------------------
 * Command handlers
 * ---------------------------------------------*/

static bool i2cScan(uint8_t argc, char *argv[])
{
	(void)argv;

	if( argc != SCLII2C_COUNT_COMMANDARGS )
	{
		i2cHelp(0U, NULL);
		return false;
	}

	err_codes_t error = sc_i2cScan();
	if( error == ERR_NO_ERROR )
	{
		tm_syslog(TM_STR("[i2c] scan complete\n"));
		return true;
	}

	const tm_string_t *message = err_getMessage((error_count_t)error);
	if( message != NULL ) { tm_syslog(TM_STR("[i2c] scan error: %s\n"), message); }
	else { tm_syslog(TM_STR("[i2c] scan error\n")); }
	return false;
}

static bool i2cHelp(uint8_t argc, char *argv[])
{
	(void)argc;
	(void)argv;

	tm_syslog(TM_STR("[i2c] usage:\n"));
	tm_syslog(TM_STR("\ti2c scan\n"));
	tm_syslog(TM_STR("\ti2c help\n"));
	return true;
}
