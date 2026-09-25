/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file options.c
 * @brief options implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "options.h"

#include <errno.h>
#include <limits.h>
#include <sys/stat.h>

#include "fileUtility.h"
#include "tokenizer.h"

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void setErrorsFile(const char *value, options_list_t *opt);
static void setInitrcFile(const char *value, options_list_t *opt);
static void setParseTagFile(const char *value, options_list_t *opt);
static void setGpioSignalsFile(const char *value, options_list_t *opt);
static void setWireGpioFile(const char *value, options_list_t *opt);
static void setGeneratedPath(const char *value, options_list_t *opt);
static void setSourcePath(const char *value, options_list_t *opt);
static void setErrorCount(const char *value, options_list_t *opt);

/* -----------------------------------------------
 * Option dispatch table
 * ---------------------------------------------*/

#define HAVE_OPTIONS(X)                                          \
	X(HAVE_ERROR_COUNT, "--error_count", setErrorCount)          \
	X(HAVE_ERRORS, "--errors", setErrorsFile)                    \
	X(HAVE_INITRC, "--initrc", setInitrcFile)                    \
	X(HAVE_PARSETAG, "--parsetag", setParseTagFile)              \
	X(HAVE_GPIO_SIGNALS, "--gpio_signals", setGpioSignalsFile)   \
	X(HAVE_WIRE_GPIO, "--wire_gpio", setWireGpioFile)            \
	X(HAVE_GENERATED_PATH, "--generated_path", setGeneratedPath) \
	X(HAVE_SOURCE_PATH, "--source_path", setSourcePath)

static const struct
{
	const char *name;
	void (*func)(const char *value, options_list_t *opt);
} options_cmds[] = {
#define X(e, s, f) {(s), (f)},
	HAVE_OPTIONS(X)
#undef X
		{NULL, NULL}};

enum
{
#define X(e, s, f) e,
	HAVE_OPTIONS(X)
#undef X
		HAVE_COUNT
};

static const char *have_to_string[HAVE_COUNT] = {
#define X(e, s, f) [e] = (s),
	HAVE_OPTIONS(X)
#undef X
};

static int have_options_count[HAVE_COUNT];

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static void setFileName(char *destination, const size_t destination_size, const char *value)
{
	const size_t value_length = strlen(value);

	if( autoCodeBufferStringFits(value, destination_size) == false )
	{
		AUTOCODE_MSG_ERROR("option value is too long (maximum %zu characters)",
						   destination_size - 1U);

		return;
	}

	memcpy(destination, value, value_length + 1U);
}

static const char *string_from_have(const int id)
{
#define X(e, s, f) \
	if( id == (e) ) { return have_to_string[e]; }
	HAVE_OPTIONS(X)
#undef X
	return NULL;
}

static void setErrorsFile(const char *value, options_list_t *opt)
{
	setFileName(opt->file_errors_list, sizeof(opt->file_errors_list), value);
	have_options_count[HAVE_ERRORS]++;
}

static void setErrorCount(const char *value, options_list_t *opt)
{
	char *end = NULL;
	errno = 0;
	const unsigned long error_count = strtoul(value, &end, 10);

	if( (errno != 0) || (end == value) || (*end != '\0') || (value[0] == '-') ||
		(error_count > UINT_MAX) )
	{
		AUTOCODE_MSG_ERROR("invalid --error_count value <%s>", value);

		return;
	}

	opt->error_count = (unsigned int)error_count;
	have_options_count[HAVE_ERROR_COUNT]++;
}

static void setInitrcFile(const char *value, options_list_t *opt)
{
	setFileName(opt->file_initrc_list, sizeof(opt->file_initrc_list), value);
	have_options_count[HAVE_INITRC]++;
}

static void setParseTagFile(const char *value, options_list_t *opt)
{
	setFileName(opt->file_parsetag_list, sizeof(opt->file_parsetag_list), value);
	have_options_count[HAVE_PARSETAG]++;
}

static void setGeneratedPath(const char *value, options_list_t *opt)
{
	setFileName(opt->generated_path, sizeof(opt->generated_path), value);
	have_options_count[HAVE_GENERATED_PATH]++;
}

static void setGpioSignalsFile(const char *value, options_list_t *opt)
{
	setFileName(opt->file_gpio_signals, sizeof(opt->file_gpio_signals), value);
	have_options_count[HAVE_GPIO_SIGNALS]++;
}

static void setWireGpioFile(const char *value, options_list_t *opt)
{
	setFileName(opt->file_wire_gpio, sizeof(opt->file_wire_gpio), value);
	have_options_count[HAVE_WIRE_GPIO]++;
}

static void setSourcePath(const char *value, options_list_t *opt)
{
	struct stat status;
	if( (stat(value, &status) != 0) || !S_ISDIR(status.st_mode) )
	{
		AUTOCODE_MSG_ERROR("invalid --source_path directory <%s>", value);
		return;
	}

	setFileName(opt->source_path, sizeof(opt->source_path), value);
	have_options_count[HAVE_SOURCE_PATH]++;
}

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

int options(const char *file_name, options_list_t *opt)
{
	// Initialise required options
	for( int i = 0; i < HAVE_COUNT; i++ ) { have_options_count[i] = 0; }

	// Process options from files
	file_t file;
	fileInit(&file);
	file.name = (char *)file_name;
	if( fileOpen(&file, "r", FILE_READONLY, __FILE__, __LINE__) != 0 ) { return -1; }

	int file_line_number = 0;
	tokenizer_t tok = {0};
	file_get_line_result_t line_result;
	AUTOCODE_MSG_INFO("read file %s", file_name);
	while( (line_result = fileGetLine(&file, tok.line, sizeof(tok.line))) == FILE_GET_LINE_SUCCESS )
	{
		file_line_number++;
		if( tokenizer(&tok) != 0 ) { continue; }

		if( (tok.count != 0) && (tok.tokens[0][0] != '#') )
		{
			if( tok.count == 2 )
			{
				AUTOCODE_MSG_INFO("parsing %s = %s", tok.tokens[0], tok.tokens[1]);

				int err = optionCmdDispatch(tok.tokens[0], tok.tokens[1], opt);

				if( err != 0 )
				{
					AUTOCODE_MSG_ERROR(
						"unknown option [%s:%i] %s\n", file.name, file_line_number, tok.tokens[0]);
				}
			}
			else
			{
				AUTOCODE_MSG_ERROR("wrong token count [%s:%i] is %i, should be 2",
								   file.name,
								   file_line_number,
								   tok.count);
			}
		}
	}
	if( line_result == FILE_GET_LINE_ERROR )
	{
		AUTOCODE_MSG_ERROR("reading file <%s> after line %i", file.name, file_line_number);
	}
	tokenizerFree(&tok);
	int result = (line_result == FILE_GET_LINE_ERROR) ? -1 : 0;
	if( fileClose(&file, __FILE__, __LINE__) != 0 ) { result = -1; }
	if( result != 0 ) { return result; }

	// Test required options
	for( int i = 0; i < HAVE_COUNT; i++ )
	{
		if( have_options_count[i] == 0 )
		{
			AUTOCODE_MSG_ERROR("required autoCode option %s is not set", string_from_have(i));
		}

		if( have_options_count[i] > 1 )
		{
			AUTOCODE_MSG_ERROR("required autoCode option %s is multiple set", string_from_have(i));
		}
	}
	return 0;
}
