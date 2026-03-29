/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file options.c
 * @brief autoCode options parser
 *
 */

#include "options.h"

#include "tokenizer.h"

#define HAVE_OPTIONS(X)                  \
	X(HAVE_TM_VER, "--tm_ver")           \
	X(HAVE_TM_BUILD, "--tm_build")       \
	X(HAVE_ARCH, "--arch")               \
	X(HAVE_MCU, "--mcu")                 \
	X(HAVE_BOARD, "--board")             \
	X(HAVE_ERRORS, "--errors")           \
	X(HAVE_HAL_USER, "--files_hal_user") \
	X(HAVE_HAL_SYSTEM, "--files_hal_system")

enum
{
#define X(e, s) e,
	HAVE_OPTIONS(X)
#undef X
		HAVE_COUNT
} have_options_id;

static const char *have_to_string[HAVE_COUNT] = {
#define X(e, s) [e] = (s),
	HAVE_OPTIONS(X)
#undef X
};

static int have_options_count[HAVE_COUNT];

static const char *string_from_have(const int id)
{
#define X(e, s) \
	if( id == (e) ) { return have_to_string[e]; }
	HAVE_OPTIONS(X)
#undef X
	return NULL;
}
static void funcTmVer(const char *value, options_list_t *opt)
{
	strncpy(opt->tm_ver, value, BYTE_INDEX);
	have_options_count[HAVE_TM_VER]++;
}

static void funcTmBuild(const char *value, options_list_t *opt)
{
	strncpy(opt->tm_build, value, BYTE_INDEX);
	have_options_count[HAVE_TM_BUILD]++;
}
static void funcArch(const char *value, options_list_t *opt)
{
	strncpy(opt->arch_name, value, BYTE_INDEX);
	have_options_count[HAVE_ARCH]++;
}

static void funcMcu(const char *value, options_list_t *opt)
{
	strncpy(opt->mcu_name, value, BYTE_INDEX);
	have_options_count[HAVE_MCU]++;
}

static void funcBoard(const char *value, options_list_t *opt)
{
	strncpy(opt->board_name, value, BYTE_INDEX);
	have_options_count[HAVE_BOARD]++;
}

static void funcErrors(const char *value, options_list_t *opt)
{
	strncpy(opt->errors_file, value, BYTE_INDEX);
	have_options_count[HAVE_ERRORS]++;
}

static void funcHalUser(const char *value, options_list_t *opt)
{
	strncpy(opt->files_hal_user, value, BYTE_INDEX);
	have_options_count[HAVE_HAL_USER]++;
}

static void funcHalSystem(const char *value, options_list_t *opt)
{
	strncpy(opt->files_hal_system, value, BYTE_INDEX);
	have_options_count[HAVE_HAL_SYSTEM]++;
}

static const struct
{
	const char *name;
	void (*func)(const char *value, options_list_t *opt);

} options_cmds[] = {{"--tm_ver", funcTmVer},
					{"--tm_build", funcTmBuild},
					{"--arch", funcArch},
					{"--mcu", funcMcu},
					{"--board", funcBoard},
					{"--errors", funcErrors},
					{"--files_hal_user", funcHalUser},
					{"--files_hal_system", funcHalSystem},
					{NULL, NULL}};

static int optionCmdDispatch(const char *cmd, const char *value, options_list_t *opt)
{
	for( int i = 0; options_cmds[i].name != NULL; i++ )
	{
		if( strcmp(cmd, options_cmds[i].name) == 0 )
		{
			(*options_cmds[i].func)(value, opt);
			return 0;
		}
	}
	return -1;
}

void options(const char *file_name, options_list_t *opt)
{
	// initialise required options
	for( int i = 0; i < HAVE_COUNT; i++ ) { have_options_count[i] = 0; }

	// proceed options from files
	file_t file;
	fileInit(&file);
	file.name = (char *)file_name;
	fileOpen(&file, "r", FILE_READONLY, __FILE__, __LINE__);

	int file_line_number = 0;
	tokenizer_t tok;

	while( fgets(tok.line, TOKEN_LINE_SIZE_MAX, file.stream) )
	{
		file_line_number++;
		tokenizer(&tok);

		if( (tok.count != 0) && (tok.tokens[0][0] != '#') )
		{
			if( tok.count == 2 )
			{
				msgInfo(" parsing %s = %s", tok.tokens[0], tok.tokens[1]);

				int err = optionCmdDispatch(tok.tokens[0], tok.tokens[1], opt);

				if( err != 0 )
				{
					msgError(
						"unknown option [%s:%i] %s\n", file.name, file_line_number, tok.tokens[0]);
					exit(1);
				}
			}
			else
			{
				msgError("wrong token count [%s:%i] is %i, should be 2",
						 file.name,
						 file_line_number,
						 tok.count);
				exit(1);
			}
		}
	}

	// test required options
	for( int i = 0; i < HAVE_COUNT; i++ )
	{
		if( have_options_count[i] == 0 )
		{
			msgError("required option %s is not set", string_from_have(i));
			exit(1);
		}

		if( have_options_count[i] > 1 )
		{
			msgError("required option %s is multiple set", string_from_have(i));
			exit(1);
		}
	}
}
