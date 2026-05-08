/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file autoCode.c
 * @brief autoCode implementation.
 *
 * - Simple and reliable, read plain text init.rc files
 * - Write code for #include / static allocation / initialisation directly in code
 *
 * @note
 * tag format is a one-line C comment: // [autoCode_tag] <object> <action>
 * end tag zone: // [/tag]
 *
 */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * autoCode is a critical component: if it generates incorrect code,
 * TaskMate may still compile but will behave unpredictably at runtime.
 * Any change to autoCode must be considered system-critical and tested
 * accordingly.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * */

#include "autoCode.h"

#include "fileUtility.h"
#include "globalError.h"
#include "options.h"
#include "parseInitrc.h"
#include "parseTag.h"
#include "printModules.h"
#include "tokenizer.h"

static void setupDB(modules_database_t *data_base);
static void threadCountLevel(modules_database_t *data_base);

int main(int argn, const char *argv[])
{
	// get options
	if( argn != 2 )
	{
		msgError("autoCode bad argn (is %i, not 2)\n\tuse autoCode configuration_file", argn);
		exit(1);
	}

	options_list_t auto_options;
	options(argv[1], &auto_options);

	// global error system
	error_catalog_t errors_catalog;
	globalError(auto_options.file_errors_list, &errors_catalog);

	// setup data base
	modules_database_t data_base;
	setupDB(&data_base);

	// read init.rc file and store data in data base
	file_t finitrc;
	fileInit(&finitrc);
	finitrc.name = auto_options.file_initrc_list;
	fileOpen(&finitrc, "r", FILE_READONLY, __FILE__, __LINE__);

	tokenizer_t tok;

	while( fgets(tok.line, TOKEN_LINE_SIZE_MAX, finitrc.stream) )
	{
		tokenizer(&tok);
		parseInitrc(&data_base, tok.tokens[0]);
	}
	fileClose(&finitrc, __FILE__, __LINE__);

	// count thread for each level
	threadCountLevel(&data_base);

	// parse tag and generate code
	file_t ftag;
	fileInit(&ftag);
	ftag.name = auto_options.file_parsetag_list;
	fileOpen(&ftag, "r", FILE_READONLY, __FILE__, __LINE__);

	parseTagInit();
	while( fgets(tok.line, TOKEN_LINE_SIZE_MAX, ftag.stream) )
	{
		tokenizer(&tok);
		parseTag(&data_base, tok.tokens[0], &errors_catalog, &auto_options);
	}
	fileClose(&ftag, __FILE__, __LINE__);
	parseTagHave();

	// print all info about modules
	printModules(&data_base);
	filePrintModified();
	return 0;
}

static void setupDB(modules_database_t *data_base)
{
	for( int i = 0; i < TM_MOD_TYPE_COUNT; i++ )
	{
		for( int j = 0; j < RUN_LEVEL_COUNT; j++ ) { data_base->run_level_module_count[i][j] = 0; }
	}

	for( int i = 0; i < TM_MOD_TYPE_COUNT; i++ )
	{
		data_base->modules_type[i].modules_count = 0;
		for( int j = 0; j < TM_MOD_COUNT_MAX; j++ )
		{
			data_base->modules_type[i].modules[j].set_runlevel = 0;
			data_base->modules_type[i].modules[j].set_type = 0;
		}
	}
}

static void threadCountLevel(modules_database_t *data_base)
{
	for( int level = 0; level < RUN_LEVEL_COUNT; level++ )
	{

		// count thread for each run level
		int thread_count = 0;

		for( int i = 1; i <= level; i++ )
		{
			thread_count += data_base->run_level_module_count[TM_MOD_THREAD_ID][i];
		}

		data_base->threads_count[level] = thread_count;
	}
}
