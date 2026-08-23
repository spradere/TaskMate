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
#include "system/services/commands/thread.h"
#include "system/services/msg.h"
#include "system/sysCall/sysCall.h"
#include "tm_libc/tm_string.h"
#include "tm_libc/tm_syslog.h"

#define SCLI_LINE_SIZE 64
#define SCLI_ARGUMENT_COUNT_MAX 4

typedef bool (*scli_command_func_t)(uint8_t argc, char *argv[]);

typedef struct
{
	const char *name;
	scli_command_func_t func;
} scli_command_t;

static uint8_t scli_msg_channel;
static char scli_line[SCLI_LINE_SIZE];
static uint8_t scli_line_length;
static bool scli_line_overflow;

static void scliRead(void);
static void scliLineProcess(void);
static uint8_t scliTokenize(char *line, char *argv[]);
static bool scliCommandDispatch(uint8_t argc, char *argv[]);
static bool scliStringEquals(const char *left, const char *right);

static const scli_command_t scli_commands[] = {
	{"thread", thread},
	{0, 0},
};

void scli(void)
{

	if( msgRequestChannel(&scli_msg_channel) == ERR_NO_ERROR )
	{
		msgWriteText(scli_msg_channel, TM_STR("[scli] ready to work\n"), MSG_TO_USART);
	}

	while( 1 )
	{
		scliRead();

		sc_threadSetSTC(100);
		while( sc_threadGetSTC() > 0 ) { sc_coopYield(); };
	}
}

static void scliRead(void)
{
	uint8_t data;

	while( hal_usartRead(&data) == ERR_NO_ERROR )
	{
		if( (data == '\r') || (data == '\n') )
		{
			if( scli_line_overflow )
			{
				tm_syslog(TM_STR("[scli] error: command too long\n"));
				scli_line_overflow = false;
				scli_line_length = 0;
			}
			else if( scli_line_length > 0 ) { scliLineProcess(); }
			continue;
		}

		if( (data == '\b') || (data == 0x7f) )
		{
			if( scli_line_length > 0 ) { scli_line_length--; }
			continue;
		}

		if( (data < ' ') || (data > '~') ) { continue; }

		if( scli_line_length < (SCLI_LINE_SIZE - 1) )
		{
			scli_line[scli_line_length++] = (char)data;
		}
		else { scli_line_overflow = true; }
	}
}

static void scliLineProcess(void)
{
	char *argv[SCLI_ARGUMENT_COUNT_MAX];

	scli_line[scli_line_length] = 0;
	uint8_t argc = scliTokenize(scli_line, argv);
	if( (argc > 0) && !scliCommandDispatch(argc, argv) )
	{
		tm_string_t command = TM_STR_RAM(argv[0]);
		tm_syslog(TM_STR("[scli] error: unknown command %s\n"), &command);
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
		if( scliStringEquals(argv[0], scli_commands[i].name) )
		{
			scli_commands[i].func(argc, argv);
			return true;
		}
	}

	return false;
}

static bool scliStringEquals(const char *left, const char *right)
{
	if( (left == 0) || (right == 0) ) { return false; }

	while( *left == *right )
	{
		if( *left == 0 ) { return true; }
		left++;
		right++;
	}

	return false;
}
