/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file i2c.c
 * @brief I2C command implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "i2c.h"

#include "interfaces/tm_define.h"
#include "system/sysCall/sc_errors.h"
#include "system/sysCall/sc_driver.h"
#include "tmLibc/tm_string.h"
#include "tmLibc/tm_syslog.h"

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
	{0, 0},
};

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Command dispatch
 * ---------------------------------------------*/

bool i2c(uint8_t argc, char *argv[])
{
	if( argc < 2 )
	{
		i2cHelp(0, NULL);
		return false;
	}

	for( uint8_t i = 0; i2c_cmd[i].name != 0; i++ )
	{
		if( tm_strncmp(
				sc_stringFromBuffer(argv[1]),
				sc_stringFromBuffer(i2c_cmd[i].name),
				TM_STRING_SIZE_MAX) == 0 )
		{
			return i2c_cmd[i].func(argc, argv);
		}
	}

	i2cHelp(0, NULL);
	return false;
}

/* -----------------------------------------------
 * Command handlers
 * ---------------------------------------------*/

static bool i2cScan(uint8_t argc, char *argv[])
{
	(void)argv;

	if( argc != 2 )
	{
		i2cHelp(0, NULL);
		return false;
	}

	err_codes_t error = sc_i2cScan();
	if( error == ERR_NO_ERROR )
	{
		tm_syslog(TM_STR("[i2c] scan complete\n"));
		return true;
	}

	const tm_string_t *message = err_getMessage((uint8_t)error);
	if( message != 0 ) { tm_syslog(TM_STR("[i2c] scan error: %s\n"), message); }
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
