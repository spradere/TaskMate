/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file parseTag.c
 * @brief parse tag implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "parseTag.h"

#include <stdbool.h>

#include "fileUtility.h"
#include "tagWriters/tagWriters.h"
#include "tokenizer.h"

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static int generatedFileName(char *file_name, size_t file_name_size, const char *generated_path,
							 const char *tag);

/* -----------------------------------------------
 * Tag dispatch table
 * ---------------------------------------------*/

#define HAVE_TAG(X)                                                                     \
	X(HAVE_THREAD_STACKS, "thread_stacks", tagWriterWriteThreadStacks)                  \
	X(HAVE_THREADS_ALLOC, "threads_alloc", tagWriterWriteThreadsAlloc)                  \
	X(HAVE_DRIVERS_ALLOC, "drivers_alloc", tagWriterWriteDriversAlloc)                  \
	X(HAVE_THREAD_NAME_CATALOG, "thread_name_catalog", tagWriterWriteThreadNameCatalog) \
	X(HAVE_DRIVER_NAME_CATALOG, "driver_name_catalog", tagWriterWriteDriverNameCatalog) \
	X(HAVE_DRIVER_HAVE, "driver_have", tagWriterWriteDriverHave)                        \
	X(HAVE_ERROR_ENUM, "error_enum", tagWriterWriteErrorEnum)                           \
	X(HAVE_ERROR_CATALOG, "error_catalog", tagWriterWriteErrorCatalog)                  \
	X(HAVE_MOD_COUNT, "modules_count", tagWriterWriteModulesCount)                      \
	X(HAVE_THREADS_LIST, "threads_list", tagWriterWriteThreadsList)                     \
	X(HAVE_DRIVERS_LIST, "drivers_list", tagWriterWriteDriversList)                     \
	X(HAVE_GPIO_SIGNALS, "gpio_signals", tagWriterWriteGpioSignals)                     \
	X(HAVE_WIRE_GPIO, "wire_gpio", tagWriterWriteWireGpio)                              \
	X(HAVE_SCLI_COMMANDS, "scli_commands", tagWriterWriteScliCommands)

enum
{
#define X(e, s, f) e,
	HAVE_TAG(X)
#undef X
		HAVE_COUNT
};

static const struct
{
	int id;
	const char *tag;
	void (*func)(const tag_writer_context_t *context);
} tags_cmds[] = {
#define X(e, s, f) {(e), (s), (f)},
	HAVE_TAG(X)
#undef X
		{0, NULL, NULL}};

static const char *have_to_string[HAVE_COUNT] = {
#define X(e, s, f) [e] = (s),
	HAVE_TAG(X)
#undef X
};

static int have_tag_count[HAVE_COUNT];

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Tag parsing and validation
 * ---------------------------------------------*/

static const char *string_from_have(const int id)
{
#define X(e, s, f) \
	if( id == (e) ) { return have_to_string[e]; }
	HAVE_TAG(X)
#undef X
	return NULL;
}

static int tagCmdDispatch(const char *cmd, const tag_writer_context_t *context)
{
	for( int i = 0; tags_cmds[i].tag != NULL; i++ )
	{
		if( strcmp(cmd, tags_cmds[i].tag) == 0 )
		{
			(*tags_cmds[i].func)(context);
			// A writer with a file error has not satisfied the required tag.
			if( !*context->file_error ) { have_tag_count[tags_cmds[i].id]++; }
			return 0;
		}
	}
	return -1;
}

static int generatedFileName(char *file_name, const size_t file_name_size,
							 const char *generated_path, const char *tag)
{
	const int length = snprintf(file_name, file_name_size, "%s/%s.inc", generated_path, tag);

	if( (length < 0) || ((size_t)length >= file_name_size) )
	{
		AUTOCODE_MSG_ERROR("generated file name is too long for tag <%s>", tag);
		return -1;
	}

	return 0;
}

void parseTagInit(void)
{
	// Count tags across every source file in the tag list.
	for( int i = 0; i < HAVE_COUNT; i++ ) { have_tag_count[i] = 0; }
}

int parseTag(modules_database_t *data_base, const char *file_name, const error_catalog_t *errors,
			 const options_list_t *auto_options)
{
	// Stage the tagged source alongside its generated fragment.
	AUTOCODE_MSG_INFO("open <%s>", file_name);

	file_t file_src;
	fileInit(&file_src);
	file_src.name = (char *)file_name;
	if( fileOpen(&file_src, "r", FILE_READONLY) != FILE_UTILITY_OK )
	{
		AUTOCODE_MSG_ERROR("opening file <%s>", file_name);
		return -1;
	}

	file_t file_tmp;
	fileInit(&file_tmp);
	if( fileMakeTmp(file_src.name, &file_tmp) != 0 )
	{
		AUTOCODE_MSG_ERROR("creating temporary file for <%s>", file_name);
		if( fileClose(&file_src) != FILE_UTILITY_OK )
		{
			AUTOCODE_MSG_ERROR("closing file <%s>", file_name);
		}
		return -1;
	}

	bool file_error = false;
	tag_writer_context_t context = {.data_base = data_base,
									.file = NULL,
									.errors = errors,
									.auto_options = auto_options,
									.file_error = &file_error};

	// Copy text outside tagged regions; replace each region with an include.
	int tag_section = 0;
	int file_line_number = 0;
	tokenizer_t tok = {0};
	char line[TOKEN_LINE_SIZE_MAX];
	file_get_line_result_t line_result;

	while( (line_result = fileGetLine(&file_src, tok.line, sizeof(tok.line))) ==
		   FILE_GET_LINE_SUCCESS )
	{
		file_line_number++;
		// Tokenization changes the line; preserve the exact source text for copying.
		snprintf(line, sizeof(line), "%s", tok.line);
		tokenizer_err_t token_error = tokenizer(&tok);
		if( token_error != TOK_ERR_NOERR )
		{
			AUTOCODE_MSG_ERROR("tokenizer [%s:%i]: %s",
							   file_src.name,
							   file_line_number,
							   tokenizerErrorMessage(token_error));
			continue;
		}

		if( (tok.count >= 2) && !(strcmp(tok.tokens[0], "//")) &&
			!(strcmp(tok.tokens[1], "[autoCode_tag]")) )
		{
			// A start tag needs one name and cannot nest inside another region.
			if( tok.count != 3 )
			{
				AUTOCODE_MSG_ERROR(
					"token count != 3 tok.line [%s:%i] %s", file_src.name, file_line_number, line);
				break;
			}

			if( tag_section == 1 )
			{
				AUTOCODE_MSG_ERROR(
					"Start new tag section without previous end tag [/tag] [%s:%i] %s",
					file_src.name,
					file_line_number,
					line);
				break;
			}

			AUTOCODE_MSG_INFO("found tag %s", tok.tokens[2]);

			// The source keeps its tag anchor and includes the generated fragment.
			fprintf(file_tmp.stream, "%s", line);

			fprintf(file_tmp.stream, "#include \"%s.inc\"\n", tok.tokens[2]);

			char generated_file_name[AC_BUFFER_SIZE];
			if( generatedFileName(generated_file_name,
								  sizeof(generated_file_name),
								  auto_options->generated_path,
								  tok.tokens[2]) != 0 )
			{
				file_error = true;
				break;
			}

			file_t file_generated;
			fileInit(&file_generated);
			// Stage each fragment separately from its tagged source file.
			if( fileMakeTmp(generated_file_name, &file_generated) != 0 )
			{
				AUTOCODE_MSG_ERROR("creating temporary file for <%s>", generated_file_name);
				file_error = true;
				break;
			}
			context.file = file_generated.stream;

			// Keep generated diagnostics separate from the tagged source line numbers.
			fprintf(context.file, "/*\n");
			fprintf(context.file, "* do not edit code in this file \n");
			fprintf(context.file, "* code generated by autoCode, any change will be lost\n");
			fprintf(context.file, "*/\n");

			fprintf(context.file, "\n#line %i\n\n", AC_GENERATED_LINE_START);

			tag_section = 1;

			// Finish the writer's stream before accepting the generated tag.
			int err = tagCmdDispatch(tok.tokens[2], &context);
			if( fileClose(&file_generated) != FILE_UTILITY_OK )
			{
				AUTOCODE_MSG_ERROR("closing generated file <%s>", generated_file_name);
				file_error = true;
			}
			if( file_error )
			{
				tag_section = 0;
				break;
			}

			if( err != 0 )
			{
				AUTOCODE_MSG_ERROR(
					"unknown tag [%s:%i] %s\n", file_name, file_line_number, tok.tokens[2]);
			}
		}

		// Closing a region resumes copying the source text unchanged.
		if( (tok.count >= 2) && !(strcmp(tok.tokens[0], "//")) &&
			!(strcmp(tok.tokens[1], "[/tag]")) )
		{
			AUTOCODE_MSG_INFO("end tag");
			tag_section = 0;
		}

		if( tag_section == 0 ) { fprintf(file_tmp.stream, "%s", line); }
	}
	if( line_result == FILE_GET_LINE_ERROR )
	{
		AUTOCODE_MSG_ERROR("reading file <%s> after line %i", file_src.name, file_line_number);
		file_error = true;
	}
	tokenizerFree(&tok);

	// Report an unterminated region, then close both source and staged output.
	if( tag_section == 1 )
	{
		AUTOCODE_MSG_ERROR("missing end tag [/tag] [%s:%i]", file_src.name, file_line_number);
	}
	if( fileClose(&file_src) != FILE_UTILITY_OK )
	{
		AUTOCODE_MSG_ERROR("closing file <%s>", file_name);
		file_error = true;
	}
	if( fileClose(&file_tmp) != FILE_UTILITY_OK )
	{
		AUTOCODE_MSG_ERROR("closing temporary file for <%s>", file_name);
		file_error = true;
	}
	return file_error ? -1 : 0;
}

void parseTagHave(void)
{
	// Every known tag must have produced exactly one fragment.
	for( int i = 0; i < HAVE_COUNT; i++ )
	{
		if( have_tag_count[i] == 0 )
		{
			AUTOCODE_MSG_ERROR("required autoCode tag %s is not set", string_from_have(i));
		}

		if( have_tag_count[i] > 1 )
		{
			AUTOCODE_MSG_ERROR("required autoCode tag %s is multiple set", string_from_have(i));
		}
	}
}
