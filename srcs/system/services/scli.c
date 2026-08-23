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

#include "scli.h"

#include <stdbool.h>
#include <stdint.h>

#include "hal/public/usart.h"
#include "system/services/commands/driver.h"
#include "system/services/commands/thread.h"
#include "system/services/msg.h"
#include "system/sysCall/sysCall.h"
#include "tm_libc/tm_string.h"
#include "tm_libc/tm_syslog.h"

#define SCLI_LINE_SIZE 64
#define SCLI_ARGUMENT_COUNT_MAX 4

typedef bool (*scli_cmd_func_t)(uint8_t argc, char *argv[]);

typedef struct
{
	const char *name;
	scli_cmd_func_t func;
} scli_cmd_t;

static char scli_line[SCLI_LINE_SIZE];
static uint8_t scli_line_length;

static void scliRead(void);
static void scliLineProcess(void);
static uint8_t scliTokenize(char *line, char *argv[]);
static bool scliCommandDispatch(uint8_t argc, char *argv[]);

static const scli_cmd_t scli_commands[] = {
	{"driver", driver},
	{"thread", thread},
	{0, 0},
};

void scli(void)
{
	while( 1 )
	{
		scliRead();
		sc_threadSetSTC(50);
		while( sc_threadGetSTC() > 0 ) { sc_coopYield(); };
	}
}

static void scliRead(void)
{
	uint8_t data;

	if( hal_usartTestBufferRx() != ERR_HAL_USART_RX_BUFFER_EMPTY )
	{
		uint8_t i = 0;

		while( (hal_usartRead(&data) == ERR_NO_ERROR) && (i < (sizeof(scli_line) - 1)) )
		{
			scli_line[i++] = (char)data;
		}
		scli_line[i] = 0;

		scliLineProcess();
	}
}

static void scliLineProcess(void)
{
	char *argv[SCLI_ARGUMENT_COUNT_MAX];

	uint8_t argc = scliTokenize(scli_line, argv);
	if( (argc > 0) && !scliCommandDispatch(argc, argv) )
	{
		tm_string_t command = TM_STR_RAM(argv[0]);
		tm_syslog(TM_STR("[scli] error: unknown command %s\n"), &command);
		for( uint8_t i = 0; scli_commands[i].name != 0; i++ )
		{
			tm_string_t command_name = TM_STR_RAM(scli_commands[i].name);
			tm_syslog(TM_STR("\tcmd %s\n"), &command_name);
		}
	}
	scli_line_length = 0;
}

static uint8_t scliTokenize(char *line, char *argv[])
{
	uint8_t argc = 0;
	char *cursor = line;

	while( *cursor != 0 )
	{
		while( (*cursor == ' ') || (*cursor == '\t') ) { cursor++; }
		if( *cursor == 0 ) { break; }
		if( argc == SCLI_ARGUMENT_COUNT_MAX ) { return argc; }

		argv[argc++] = cursor;
		while( (*cursor != 0) && (*cursor != ' ') && (*cursor != '\t') ) { cursor++; }
		if( *cursor != 0 ) { *cursor++ = 0; }
	}

	return argc;
}

static bool scliCommandDispatch(uint8_t argc, char *argv[])
{
	for( uint8_t i = 0; scli_commands[i].name != 0; i++ )
	{
		if( tm_strncmp(
				TM_STR_RAM(argv[0]), TM_STR_RAM(scli_commands[i].name), TM_STRING_SIZE_MAX) == 0 )
		{
			scli_commands[i].func(argc, argv);
			return true;
		}
	}

	return false;
}
