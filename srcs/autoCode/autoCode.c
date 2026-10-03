/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file autoCode.c
 * @brief auto code implementation.
 *
 * - Simple and reliable; reads plain-text init.rc files
 * - Writes generated fragments included from tagged source regions
 *
 * @note
 * Tag format is a one-line C comment: // [autoCode_tag] <object> <action>
 * End-tag format: // [/tag]
 *
 */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * autoCode is a critical component: if it generates incorrect code,
 * TaskMate may still compile but will behave unpredictably at runtime.
 * Any change to autoCode must be considered system-critical and tested
 * accordingly.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "autoCode.h"

#include "fileUtility.h"
#include "globalError.h"
#include "options.h"
#include "parseInitrc.h"
#include "parseTag.h"
#include "printModules.h"
#include "tokenizer.h"

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void setupDatabase(modules_database_t *data_base);

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

#define ERROR_COUNT_NORMAL_MODE 1U
#define ERROR_COUNT_TEST_MODE 100U

typedef enum
{
	AC_STAGE_ERRORS,
	AC_STAGE_INITRC,
	AC_STAGE_TAGS,
	AC_STAGE_COUNT
} ac_stage_t;

static unsigned int error_count = 0U;
static unsigned int error_count_maximum = ERROR_COUNT_NORMAL_MODE;

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

int main(int argc, const char *argv[])
{
	tokenizer_t tok = {0};

	// The configuration must be valid before any input list is processed.
	if( argc != 2 )
	{
		AUTOCODE_MSG_ERROR("autoCode bad argc (is %i, not 2)\n\tuse autoCode configuration_file",
						   argc);
		return EXIT_FAILURE;
	}

	options_list_t auto_options = {0};
	if( options(argv[1], &auto_options) != 0 ) { autoCodeExit(AC_FORCE_EXIT); }
	error_count_maximum =
		auto_options.test_mode ? ERROR_COUNT_TEST_MODE : ERROR_COUNT_NORMAL_MODE;
	autoCodeExit(AC_FORCE_EXIT);

	// Keep one database for all stages so tags use the validated declarations.
	modules_database_t data_base;
	setupDatabase(&data_base);

	error_catalog_t errors_catalog;
	errors_catalog.error_count = 0;

	// Process the lists in dependency order: errors, init.rc, then tags.
	for( ac_stage_t stage = AC_STAGE_ERRORS; stage < AC_STAGE_COUNT; stage++ )
	{
		file_t list_file;
		fileInit(&list_file);
		if( stage == AC_STAGE_ERRORS ) { list_file.name = auto_options.file_errors_list; }
		if( stage == AC_STAGE_INITRC ) { list_file.name = auto_options.file_initrc_list; }
		if( stage == AC_STAGE_TAGS ) { list_file.name = auto_options.file_parsetag_list; }

		if( fileOpen(&list_file, "r", FILE_READONLY) != 0 )
		{
			AUTOCODE_MSG_ERROR("opening file <%s>", list_file.name);
			autoCodeExit(AC_FORCE_EXIT);
		}

		if( stage == AC_STAGE_TAGS ) { parseTagInit(); }

		int file_line_number = 0;
		file_get_line_result_t line_result;
		while( (line_result = fileGetLine(&list_file, tok.line, sizeof(tok.line))) ==
			   FILE_GET_LINE_SUCCESS )
		{
			file_line_number++;
			tokenizer_err_t token_error = tokenizer(&tok);
			if( token_error != TOK_ERR_NOERR )
			{
				AUTOCODE_MSG_ERROR("tokenizer [%s:%i]: %s", list_file.name, file_line_number,
								   tokenizerErrorMessage(token_error));
				continue;
			}
			
			if( tok.count == 0 ) { continue; }
			
			if( (stage == AC_STAGE_ERRORS) &&
				(globalError(tok.tokens[0], &errors_catalog) != 0) )
			{
				break;
			}
			if( (stage == AC_STAGE_INITRC) &&
				(parseInitrc(&data_base, tok.tokens[0], auto_options.source_path) != 0) )
			{
				break;
			}
			if( (stage == AC_STAGE_TAGS) &&
				(parseTag(&data_base, tok.tokens[0], &errors_catalog, &auto_options) != 0) )
			{
				break;
			}
		}

		if( line_result == FILE_GET_LINE_ERROR )
		{
			AUTOCODE_MSG_ERROR("reading file <%s>", list_file.name);
		}

		if( fileClose(&list_file) != FILE_UTILITY_OK )
		{
			if( stage == AC_STAGE_ERRORS ) { AUTOCODE_MSG_ERROR("closing error list file"); }
			if( stage == AC_STAGE_INITRC ) { AUTOCODE_MSG_ERROR("closing initrc list file"); }
			if( stage == AC_STAGE_TAGS ){AUTOCODE_MSG_ERROR("closing tag list file");}
		}

		tokenizerFree(&tok);
		autoCodeExit(AC_FORCE_EXIT);	
			
		if( stage == AC_STAGE_TAGS ){parseTagHave();}
		autoCodeExit(AC_FORCE_EXIT);
	}

	// Publish only after every input stage has completed without errors.
	file_utility_err_t replace_result = fileCmpReplaceAll();
	if( replace_result != FILE_UTILITY_OK )
	{
		AUTOCODE_MSG_ERROR("publishing generated files: %s", fileUtilityErrorMessage(replace_result));
		autoCodeExit(AC_FORCE_EXIT);
	}

	// Report the declarations and the files actually changed by publication.
	printModules(&data_base);
	filePrintModified();
	return EXIT_SUCCESS;
}

void autoCodeExit(ac_error_cmd_t cmd)
{
	if( cmd == AC_INCREMENT )
	{
		error_count++;
		if( error_count >= error_count_maximum ) { exit(EXIT_FAILURE); }
	}
	if( cmd == AC_FORCE_EXIT )
	{
		if( error_count > 0 ) { exit(EXIT_FAILURE); }
	}
}

static void setupDatabase(modules_database_t *data_base)
{
	data_base->scli.count = 0;
	for( int i = 0; i < MOD_TYPE_COUNT; i++ ) { data_base->modules_type[i].modules_count = 0; }
}
