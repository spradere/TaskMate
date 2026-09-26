/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tagWriters_modules.c
 * @brief autoCode module tag writer implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "tagWriters.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void tagWriterWriteModulesList(const tag_writer_context_t *context)
{
	const module_type_t *mod = &context->data_base->modules_type[MOD_THREAD_ID];

	for( int i = 0; i < mod->modules_count; i++ )
	{
		if( mod->modules[i].subtype == THREAD_BIT_TYPE_SYS )
		{
			fprintf(context->file, "#include \"system/services/%s.h\"\n", mod->modules[i].name);
		}
		if( mod->modules[i].subtype == THREAD_BIT_TYPE_USER )
		{
			fprintf(context->file, "#include \"user/tasks/%s.h\"\n", mod->modules[i].name);
		}
	}
	fprintf(context->file, "\n");

	mod = &context->data_base->modules_type[MOD_DRIVER_ID];

	for( int i = 0; i < mod->modules_count; i++ )
	{
		fprintf(context->file, "#include \"interfaces/drv_%s.h\"\n", mod->modules[i].name);
	}
}

void tagWriterWriteModulesCount(const tag_writer_context_t *context)
{
	fprintf(context->file,
			"#define MOD_DRIVER_COUNT %i\n",
			context->data_base->modules_type[MOD_DRIVER_ID].modules_count);
	fprintf(context->file,
			"#define MOD_THREAD_COUNT %i\n",
			context->data_base->modules_type[MOD_THREAD_ID].modules_count);
}
