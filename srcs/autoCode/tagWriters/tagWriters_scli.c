/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tagWriters_scli.c
 * @brief autoCode SCLI tag writer implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "tagWriters.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void tagWriterWriteScliCommands(const tag_writer_context_t *context)
{
	for( uint8_t i = 0; i < context->data_base->scli.count; i++ )
	{
		fprintf(context->file,
				"#include \"system/services/commands/scli_%s.h\"\n",
				context->data_base->scli.commands[i].name);
	}
	fprintf(context->file, "\nstatic const scli_cmd_t scli_commands[] = {\n");
	for( uint8_t i = 0; i < context->data_base->scli.count; i++ )
	{
		fprintf(context->file,
				"\t{\"%s\", %s},\n",
				context->data_base->scli.commands[i].name,
				context->data_base->scli.commands[i].function);
	}
	fprintf(context->file, "\t{0, 0},\n");
	fprintf(context->file, "};\n");
}
