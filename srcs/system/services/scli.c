/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file scli.c
 * @brief scli implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "scli.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "system/sysCall/sc_driver.h"
#include "system/sysCall/sc_errors.h"
#include "system/sysCall/sc_string.h"
#include "system/sysCall/sc_threads.h"
#include "tmLibc/tm_string.h"
#include "tmLibc/tm_syslog.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define SCLI_LINE_SIZE 64
#define SCLI_ARGUMENT_COUNT_MAX 4
#define SCLI_POLL_STC_TICKS 50U

/* -----------------------------------------------
 * Target command table
 * ---------------------------------------------*/

typedef struct
{
	const char *name;
	bool (*func)(uint8_t argc, char *argv[]);
} scli_cmd_t;

// [autoCode_tag] scli_commands
#include "scli_commands.inc"
// [/tag]

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static char scli_line[SCLI_LINE_SIZE];
static uint8_t scli_line_length;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static err_codes_t scliRead(void);
static void scliLineProcess(void);
static uint8_t scliTokenize(char *argv[]);
static bool scliCommandDispatch(uint8_t argc, char *argv[]);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Service loop
 * ---------------------------------------------*/

void scli(void)
{
	sc_threadSetInitialized();

	while( 1U )
	{
		err_codes_t error = scliRead();
		if( error != ERR_NO_ERROR )
		{
			const tm_string_t *message = err_getMessage((uint8_t)error);
			if( message != NULL ) { tm_syslog(TM_STR("[scli] error: %s\n"), message); }
		}
		sc_threadSetSTC(SCLI_POLL_STC_TICKS);
		while( sc_threadGetSTC() > 0U ) { sc_coopYield(); };
	}
}

/* -----------------------------------------------
 * Input processing
 * ---------------------------------------------*/

static err_codes_t scliRead(void)
{
	uint8_t data = 0U;
	uint8_t i = 0U;
	err_codes_t error = ERR_NO_ERROR;

	while( i < (sizeof(scli_line) - 1U) )
	{
		error = sc_usartRead(&data);
		if( error != ERR_NO_ERROR ) { break; }
		scli_line[i++] = (char)data;
	}

	if( i > 0U )
	{
		scli_line[i] = 0U;
		scliLineProcess();
	}

	if( error == ERR_HAL_USART_RX_BUFFER_EMPTY ) { return ERR_NO_ERROR; }
	return error;
}

static void scliLineProcess(void)
{
	char *argv[SCLI_ARGUMENT_COUNT_MAX];

	uint8_t argc = scliTokenize(argv);
	if( (argc > 0U) && !scliCommandDispatch(argc, argv) )
	{
		tm_string_t command = TM_STR_RAM(argv[0U]);
		tm_syslog(TM_STR("[scli] error: unknown command %s\n"), &command);
		for( uint8_t i = 0U; scli_commands[i].name != NULL; i++ )
		{
			tm_string_t command_name = TM_STR_RAM(scli_commands[i].name);
			tm_syslog(TM_STR("\tcmd %s\n"), &command_name);
		}
	}
	scli_line_length = 0U;
}

static uint8_t scliTokenize(char *argv[])
{
	uint8_t argc = 0U;
	uint8_t index = 0U;

	while( index < sizeof(scli_line) )
	{
		if( scli_line[index] == 0U ) { return argc; }
		if( (scli_line[index] == ' ') || (scli_line[index] == '\t') )
		{
			index++;
			continue;
		}
		if( argc == SCLI_ARGUMENT_COUNT_MAX ) { return 0U; }

		argv[argc++] = &scli_line[index];
		while( index < sizeof(scli_line) )
		{
			if( scli_line[index] == 0U ) { return argc; }
			if( (scli_line[index] == ' ') || (scli_line[index] == '\t') )
			{
				scli_line[index++] = 0U;
				break;
			}
			index++;
		}
	}

	return 0U;
}

/* -----------------------------------------------
 * Command dispatch
 * ---------------------------------------------*/

static bool scliCommandDispatch(uint8_t argc, char *argv[])
{
	for( uint8_t i = 0U; scli_commands[i].name != NULL; i++ )
	{
		if( tm_strncmp(
				TM_STR_RAM(argv[0U]), TM_STR_RAM(scli_commands[i].name), TM_STRING_SIZE_MAX) == 0U )
		{
			scli_commands[i].func(argc, argv);
			return true;
		}
	}

	return false;
}
