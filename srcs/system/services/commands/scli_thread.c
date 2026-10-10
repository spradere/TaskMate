/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file scli_thread.c
 * @brief thread command implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "scli_thread.h"

#include "interfaces/tm_define.h"
#include "system/sysCall/sc_threads.h"
#include "tmLibc/tm_string.h"
#include "tmLibc/tm_syslog.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define SCLITHREAD_COUNT_COMMANDARGS 2U
#define SCLITHREAD_COUNT_STOPARGS 3U
#define SCLITHREAD_COUNT_STARTARGS 4U
#define SCLITHREAD_INDEX_NAME 2U
#define SCLITHREAD_INDEX_RUNLEVEL 3U
#define SCLITHREAD_BASE_DECIMAL 10U
#define SCLITHREAD_COUNT_RUNLEVELDIGITS 3U

/* -----------------------------------------------
 * Private types
 * ---------------------------------------------*/

typedef bool (*thread_cmd_func_t)(uint8_t argc, char *argv[]);

typedef struct
{
	const char *name;
	thread_cmd_func_t func;
} thread_cmd_t;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static bool threadStart(uint8_t argc, char *argv[]);
static bool threadStop(uint8_t argc, char *argv[]);
static bool threadList(uint8_t argc, char *argv[]);
static bool threadHelp(uint8_t argc, char *argv[]);
static bool threadRunLevelParse(const char *text, uint8_t *run_level);

/* -----------------------------------------------
 * Command table
 * ---------------------------------------------*/

static const thread_cmd_t thread_cmd[] = {
	{"start", threadStart},
	{"stop", threadStop},
	{"list", threadList},
	{"help", threadHelp},
	{NULL, NULL},
};

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Command dispatch
 * ---------------------------------------------*/

bool threadCommand(uint8_t argc, char *argv[])
{
	if( argc < SCLITHREAD_COUNT_COMMANDARGS )
	{
		threadHelp(0U, NULL);
		return false;
	}

	for( uint8_t i = 0U; thread_cmd[i].name != NULL; i++ )
	{
		if( tm_strncmp(TM_STR_RAM(argv[1U]), TM_STR_RAM(thread_cmd[i].name), TM_STRING_SIZE_MAX) ==
			0U )
		{
			return thread_cmd[i].func(argc, argv);
		}
	}

	threadHelp(0U, NULL);
	return false;
}

/* -----------------------------------------------
 * Command handlers
 * ---------------------------------------------*/

static bool threadHelp(uint8_t argc, char *argv[])
{
	(void)argc;
	(void)argv;

	tm_syslog(TM_STR("[thread] usage:\n"));

	tm_syslog(TM_STR("\tthread start <name> [runlevel:1..4]\n"));
	tm_syslog(TM_STR("\tthread stop <name>\n"));
	tm_syslog(TM_STR("\tthread list\n"));
	tm_syslog(TM_STR("\tthread help\n"));
	return true;
}

static bool threadList(uint8_t argc, char *argv[])
{
	(void)argc;
	(void)argv;

	tm_syslog(TM_STR("[thread] threads:\n"));
	const mod_count_t thread_count = sc_threadGetCount();
	for( mod_count_t id = 0U; id < thread_count; id++ )
	{
		const tm_string_t *name;
		uint8_t run_level;
		uint16_t stack_size_bytes;
		if( !sc_threadGetInfo(id, &name, &run_level, &stack_size_bytes) ) { return false; }

		(void)stack_size_bytes;
		tm_syslog(TM_STR("\t%s runlevel=%i\n"), name, run_level);
	}

	return true;
}

static bool threadStart(uint8_t argc, char *argv[])
{
	if( (argc != SCLITHREAD_COUNT_STOPARGS) && (argc != SCLITHREAD_COUNT_STARTARGS) )
	{
		threadHelp(0U, NULL);
		return false;
	}

	uint8_t run_level = 0U;
	if( (argc == SCLITHREAD_COUNT_STARTARGS) &&
		!threadRunLevelParse(argv[SCLITHREAD_INDEX_RUNLEVEL], &run_level) )
	{
		tm_syslog(TM_STR("[thread] invalid runlevel\n"));
		return false;
	}

	if( sc_threadStart(argv[SCLITHREAD_INDEX_NAME], run_level) )
	{
		tm_string_t thread_name = TM_STR_RAM(argv[SCLITHREAD_INDEX_NAME]);
		tm_syslog(TM_STR("[thread] %s started\n"), &thread_name);
		return true;
	}

	tm_string_t thread_name = TM_STR_RAM(argv[SCLITHREAD_INDEX_NAME]);
	tm_syslog(TM_STR("[thread] %s not started: bad name or runlevel\n"), &thread_name);
	return false;
}

static bool threadStop(uint8_t argc, char *argv[])
{
	if( argc != SCLITHREAD_COUNT_STOPARGS )
	{
		threadHelp(0U, NULL);
		return false;
	}

	if( sc_threadStop(argv[SCLITHREAD_INDEX_NAME]) )
	{
		tm_string_t thread_name = TM_STR_RAM(argv[SCLITHREAD_INDEX_NAME]);
		tm_syslog(TM_STR("[thread] %s stop\n"), &thread_name);
		return true;
	}

	tm_string_t thread_name = TM_STR_RAM(argv[SCLITHREAD_INDEX_NAME]);
	tm_syslog(TM_STR("[thread] name not found %s\n"), &thread_name);
	return false;
}

/* -----------------------------------------------
 * Run-level parsing
 * ---------------------------------------------*/

static bool threadRunLevelParse(const char *text, uint8_t *run_level)
{
	if( (text == NULL) || (run_level == NULL) || (text[0U] == 0U) ) { return false; }

	uint16_t value = 0U;
	uint8_t index = 0U;
	while( index < SCLITHREAD_COUNT_RUNLEVELDIGITS )
	{
		if( text[index] == 0U )
		{
			*run_level = (uint8_t)value;
			return true;
		}
		if( (text[index] < '0') || (text[index] > '9') ) { return false; }
		value = (value * SCLITHREAD_BASE_DECIMAL) + (uint8_t)(text[index] - '0');
		if( value > UINT8_MAX ) { return false; }
		index++;
	}

	if( text[index] != 0U ) { return false; }
	*run_level = (uint8_t)value;
	return true;
}
