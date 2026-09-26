/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tagWriters_errors.c
 * @brief autoCode error tag writer implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "tagWriters.h"

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static const char *errorLevelName(err_level_t level);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void tagWriterWriteErrorCatalog(const tag_writer_context_t *context)
{
	for( int i = 0; i < context->errors->error_count; i++ )
	{
		if( context->errors->catalog[i].level != ERR_LEVEL_FLOW )
		{
			fprintf(context->file,
					"TM_STR_NEW(err%i, %s);\n",
					i,
					context->errors->catalog[i].message);
		}
	}

	fprintf(context->file, "\nconst err_item_t error_catalog[] = \n{\n");

	for( int i = 0; i < context->errors->error_count; i++ )
	{
		if( context->errors->catalog[i].level == ERR_LEVEL_FLOW )
		{
			fprintf(context->file, "\t{NULL, ERR_LEVEL_FLOW},\n");
		}
		else
		{
			fprintf(context->file,
					"\t{&err%i, %s},\n",
					i,
					errorLevelName(context->errors->catalog[i].level));
		}
	}
	fprintf(context->file, "};\n");
}

static const char *errorLevelName(const err_level_t level)
{
	switch( level )
	{
		case ERR_LEVEL_FLOW:
			return "ERR_LEVEL_FLOW";
		case ERR_LEVEL_WARN:
			return "ERR_LEVEL_WARN";
		case ERR_LEVEL_FAIL:
			return "ERR_LEVEL_FAIL";
		case ERR_LEVEL_PANIC:
			return "ERR_LEVEL_PANIC";
	}

	AUTOCODE_MSG_ERROR("unknown TaskMate error level <%i>", level);

	return "ERR_LEVEL_FAIL";
}

void tagWriterWriteErrorEnum(const tag_writer_context_t *context)
{
	fprintf(context->file, "typedef enum\n");
	fprintf(context->file, "{\n");

	for( int i = 0; i < context->errors->error_count; i++ )
	{
		fprintf(context->file, "\t%s,\n", context->errors->catalog[i].name);
	}
	fprintf(context->file, "\tERROR_COUNT\n");
	fprintf(context->file, "} err_codes_t;\n\n");
}
