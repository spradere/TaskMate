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

#include "system/sysCall/sysCall.h"
#include "tm_libc/tm_string.h"
#include "tm_libc/tm_syslog.h"

typedef bool (*thread_option_func_t)(const char *name);

typedef struct
{
	const char *name;
	thread_option_func_t func;
} thread_option_t;

static bool threadStart(const char *name);
static bool threadStop(const char *name);
static bool threadStringEquals(const char *left, const char *right);

static const thread_option_t thread_options[] = {
	{"-start", threadStart},
	{"-stop", threadStop},
	{0, 0},
};

bool thread(uint8_t argc, char *argv[])
{
	if( (argc != 3) || (argv == 0) )
	{
		tm_syslog(TM_STR("[thread] bad argn, use : thread (-start | -stop) <thread_name>\n"));
		return false;
	}

	for( uint8_t i = 0; thread_options[i].name != 0; i++ )
	{
		if( threadStringEquals(argv[1], thread_options[i].name) )
		{
			return thread_options[i].func(argv[2]);
		}
	}

	tm_syslog(TM_STR("[thread] options are (-start | -stop)\n"));
	return false;
}

static bool threadStart(const char *name)
{
	if( sc_threadStart(name) )
	{
		tm_string_t thread_name = TM_STR_RAM(name);
		tm_syslog(TM_STR("[thread] %s started\n"), &thread_name);
		return true;
	}

	tm_string_t thread_name = TM_STR_RAM(name);
	tm_syslog(TM_STR("[thread] name not found %s\n"), &thread_name);
	return false;
}

static bool threadStop(const char *name)
{
	if( sc_threadStop(name) )
	{
		tm_string_t thread_name = TM_STR_RAM(name);
		tm_syslog(TM_STR("[thread] %s stop\n"), &thread_name);
		return true;
	}

	tm_string_t thread_name = TM_STR_RAM(name);
	tm_syslog(TM_STR("[thread] name not found %s\n"), &thread_name);
	return false;
}

static bool threadStringEquals(const char *left, const char *right)
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
