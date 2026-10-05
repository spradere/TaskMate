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

#include <sys/stat.h>

#include "fileUtility.h"
#include "tokenizer.h"

enum
{
	OPTIONS_COUNT_FIELDS = 2U,
	OPTIONS_COUNT_ALLOWED = 1U
};

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
static void setTestMode(const char *value, options_list_t *opt);

/* -----------------------------------------------
 * Option dispatch table
 * ---------------------------------------------*/

#define HAVE_OPTIONS(X)                                          \
	X(HAVE_TEST_MODE, "--test_mode", setTestMode)                \
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

	if( value_length >= destination_size - 1U )
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

static void setTestMode(const char *value, options_list_t *opt)
{
	if( strcmp(value, "on") == 0U )
	{
		opt->test_mode = true;
	}
	else if( strcmp(value, "off") == 0U )
	{
		opt->test_mode = false;
	}
	else
	{
		AUTOCODE_MSG_ERROR("invalid --test_mode value <%s>, expected on or off", value);
		return;
	}

	have_options_count[HAVE_TEST_MODE]++;
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
	if( (stat(value, &status) != 0U) || !S_ISDIR(status.st_mode) )
	{
		AUTOCODE_MSG_ERROR("invalid --source_path directory <%s>", value);
		return;
	}

	setFileName(opt->source_path, sizeof(opt->source_path), value);
	have_options_count[HAVE_SOURCE_PATH]++;
}

static ac_result_t optionCmdDispatch(const char *cmd, const char *value, options_list_t *opt)
{
	for( int i = 0U; options_cmds[i].name != NULL; i++ )
	{
		if( strcmp(cmd, options_cmds[i].name) == 0U )
		{
			(*options_cmds[i].func)(value, opt);
			return AC_RESULT_OK;
		}
	}
	return AC_RESULT_ERROR;
}

/*
 * Read the configuration and report all line and option-count errors in one pass.
 * Fatal file errors return AC_RESULT_ERROR; parsed options use autoCode's error count.
 */
ac_result_t options(const char *file_name, options_list_t *opt)
{
	// Count accepted values to detect both missing and repeated options.
	for( int i = 0U; i < HAVE_COUNT; i++ ) { have_options_count[i] = 0U; }

	// A malformed line reports an error without stopping later diagnostics.
	file_t file;
	fileInit(&file);
	file.name = (char *)file_name;
	if( fileOpen(&file, "r", FILEUTILITY_MODE_READONLY) != FILE_UTILITY_OK )
	{
		AUTOCODE_MSG_ERROR("opening file <%s>", file_name);
		return AC_RESULT_ERROR;
	}

	int file_line_number = 0U;
	tokenizer_t tok = {0U};
	file_get_line_result_t line_result;
	AUTOCODE_MSG_INFO("read file %s", file_name);
	while( (line_result = fileGetLine(&file, tok.line, sizeof(tok.line))) == FILE_GET_LINE_SUCCESS )
	{
		file_line_number++;
		// Continue after a bad line to report later configuration errors together.
		tokenizer_err_t token_error = tokenizer(&tok);
		if( token_error != TOK_ERR_NOERR )
		{
			AUTOCODE_MSG_ERROR("tokenizer [%s:%i]: %s",
							   file.name,
							   file_line_number,
							   tokenizerErrorMessage(token_error));
			continue;
		}

		// Effective lines contain one option and one value.
		if( (tok.count != 0U) && (tok.tokens[0U][0U] != '#') )
		{
			if( tok.count == OPTIONS_COUNT_FIELDS )
			{
				AUTOCODE_MSG_INFO("parsing %s = %s", tok.tokens[0U], tok.tokens[1U]);

				ac_result_t err = optionCmdDispatch(tok.tokens[0U], tok.tokens[1U], opt);

				if( err != AC_RESULT_OK )
				{
					AUTOCODE_MSG_ERROR(
						"unknown option [%s:%i] %s\n", file.name, file_line_number, tok.tokens[0U]);
				}
			}
			else
			{
				AUTOCODE_MSG_ERROR("wrong token count [%s:%i] is %i, should be %i",
								   file.name,
								   file_line_number,
								   tok.count, OPTIONS_COUNT_FIELDS);
			}
		}
	}
	if( line_result == FILE_GET_LINE_ERROR )
	{
		AUTOCODE_MSG_ERROR("reading file <%s> after line %i", file.name, file_line_number);
	}
	// Release token storage and close the list before checking option cardinality.
	tokenizerFree(&tok);
	ac_result_t result = (line_result == FILE_GET_LINE_ERROR) ? AC_RESULT_ERROR : AC_RESULT_OK;
	if( fileClose(&file) != FILE_UTILITY_OK )
	{
		AUTOCODE_MSG_ERROR("closing file <%s>", file_name);
		result = AC_RESULT_ERROR;
	}
	if( result != AC_RESULT_OK ) { return result; }

	// Check cardinality after the complete configuration has been read.
	for( int i = 0U; i < HAVE_COUNT; i++ )
	{
		if( have_options_count[i] == 0U )
		{
			AUTOCODE_MSG_ERROR("required autoCode option %s is not set", string_from_have(i));
		}

		if( have_options_count[i] > OPTIONS_COUNT_ALLOWED )
		{
			AUTOCODE_MSG_ERROR("required autoCode option %s is multiple set", string_from_have(i));
		}
	}
	return AC_RESULT_OK;
}
