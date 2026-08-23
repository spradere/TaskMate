/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file thread.c
 * @brief thread command implementation.
 */

#include "thread.h"

#include "interfaces/define.h"
#include "system/sysCall/sysCall.h"
#include "tm_libc/tm_string.h"
#include "tm_libc/tm_syslog.h"

typedef bool (*thread_option_func_t)(uint8_t argc, char *argv[]);

typedef struct
{
	const char *name;
	thread_option_func_t func;
} thread_option_t;

static bool threadStart(uint8_t argc, char *argv[]);
static bool threadStop(uint8_t argc, char *argv[]);
static bool threadHelp(uint8_t argc, char *argv[]);
static bool threadRunLevelParse(const char *text, uint8_t *run_level);

static const thread_option_t thread_options[] = {
	{"start", threadStart},
	{"stop", threadStop},
	{"help", threadHelp},
	{0, 0},
};

bool thread(uint8_t argc, char *argv[])
{
	if( (argc < 2) || (argv == 0) )
	{
		threadHelp(0, NULL);
		return false;
	}

	for( uint8_t i = 0; thread_options[i].name != 0; i++ )
	{
		if( tm_strncmp(
				TM_STR_RAM(argv[1]), TM_STR_RAM(thread_options[i].name), TM_STRING_SIZE_MAX) == 0 )
		{
			return thread_options[i].func(argc, argv);
		}
	}

	threadHelp(0, NULL);
	return false;
}

static bool threadHelp(uint8_t argc, char *argv[])
{
	tm_syslog(TM_STR("[thread] usage:\n"));

	tm_syslog(TM_STR("\tthread start <name> [runlevel:1..4]\n"));
	tm_syslog(TM_STR("\tthread stop <name>\n"));
	tm_syslog(TM_STR("\tthread help\n"));
	return true;
}

static bool threadStart(uint8_t argc, char *argv[])
{
	if( (argc != 3) && (argc != 4) )
	{
		threadHelp(0, NULL);
		return false;
	}

	uint8_t run_level = 0;
	if( (argc == 4) && !threadRunLevelParse(argv[3], &run_level) )
	{
		tm_syslog(TM_STR("[thread] invalid runlevel\n"));
		return false;
	}

	if( sc_threadStart(argv[2], run_level) )
	{
		tm_string_t thread_name = TM_STR_RAM(argv[2]);
		tm_syslog(TM_STR("[thread] %s started\n"), &thread_name);
		return true;
	}

	tm_string_t thread_name = TM_STR_RAM(argv[2]);
	tm_syslog(TM_STR("[thread] %s not started: bad name or runlevel\n"), &thread_name);
	return false;
}

static bool threadStop(uint8_t argc, char *argv[])
{
	if( argc != 3 )
	{
		threadHelp(0, NULL);
		return false;
	}

	if( sc_threadStop(argv[2]) )
	{
		tm_string_t thread_name = TM_STR_RAM(argv[2]);
		tm_syslog(TM_STR("[thread] %s stop\n"), &thread_name);
		return true;
	}

	tm_string_t thread_name = TM_STR_RAM(argv[2]);
	tm_syslog(TM_STR("[thread] name not found %s\n"), &thread_name);
	return false;
}

static bool threadRunLevelParse(const char *text, uint8_t *run_level)
{
	if( (text == 0) || (run_level == 0) || (*text == 0) ) { return false; }

	uint16_t value = 0;
	while( *text != 0 )
	{
		if( (*text < '0') || (*text > '9') ) { return false; }
		value = (value * 10U) + (uint8_t)(*text - '0');
		if( value > UINT8_MAX ) { return false; }
		text++;
	}

	*run_level = (uint8_t)value;
	return true;
}
