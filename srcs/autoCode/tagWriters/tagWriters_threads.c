/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tagWriters_threads.c
 * @brief autoCode thread tag writer implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "tagWriters.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void tagWriterWriteThreadStacks(const tag_writer_context_t *context)
{
	int threads_count = 1;
	const module_type_t *mod = &context->data_base->modules_type[MOD_THREAD_ID];

	for( int i = 0; i < mod->modules_count; i++ )
	{
		int thread_index = threads_count;
		if( strcmp(mod->modules[i].name, "system") == 0 ) { thread_index = 0; }
		else { threads_count++; }

		fprintf(context->file,
				"static hal_stack_word_t thread%i_stack[%u];\n",
				thread_index,
				mod->modules[i].stack_size);
	}
}

void tagWriterWriteThreadsAlloc(const tag_writer_context_t *context)
{
	int threads_count = 1;
	int thread_index;
	bool in_system = false;
	bool system_thread_found = false;
	const module_type_t *mod;

	fprintf(context->file, "\tmod_thread_item_t *mod;\n");

	mod = &context->data_base->modules_type[MOD_THREAD_ID];

	// List other threads
	for( int i = 0; i < mod->modules_count; i++ )
	{
		// The first thread must be the system thread
		if( strcmp(mod->modules[i].name, "system") == 0 )
		{
			thread_index = 0;
			in_system = true;
			system_thread_found = true;
		}
		else { thread_index = threads_count; }

		fprintf(context->file, "\n\tmod = mod_threadGetPointer(%i);\n", thread_index);
		fprintf(context->file, "\tmod->stack = thread%i_stack;\n", thread_index);
		fprintf(context->file, "\tmod->stack_size = %u;\n", mod->modules[i].stack_size);
		fprintf(context->file, "\tmod_threadStackInit(mod);\n");

		fprintf(context->file,
				"\n\thal_threadContextInit(%s, &(mod->context), "
				"&(mod->stack[mod->stack_size - MOD_STACK_CANARY_WORD_COUNT]));\n",
				mod->modules[i].name);

		fprintf(context->file, "\tmod->software_time_counter = 0;\n");
		fprintf(context->file, "\tmod->status = %i;\n", mod->modules[i].status);
		fprintf(context->file,
				"\tmod->saved_run_level = %i;\n",
				mod->modules[i].status & RL_LEVEL_MASK);
		fprintf(context->file, "\tmod->main = %s;\n", mod->modules[i].name);

		if( in_system == false ) { threads_count++; }
		in_system = false;
	}

	if( system_thread_found == false ) { AUTOCODE_MSG_ERROR("thread system was not found."); }
}

void tagWriterWriteThreadNameCatalog(const tag_writer_context_t *context)
{
	int threads_count = 1;
	const module_type_t *mod = &context->data_base->modules_type[MOD_THREAD_ID];

	for( int i = 0; i < mod->modules_count; i++ )
	{
		int thread_index = threads_count;
		if( strcmp(mod->modules[i].name, "system") == 0 ) { thread_index = 0; }
		else { threads_count++; }

		fprintf(context->file,
				"TM_STR_NEW(thread%i_name, \"%s\");\n",
				thread_index,
				mod->modules[i].name);
	}

	fprintf(context->file,
			"\nstatic const tm_string_t *const thread_name_catalog[MOD_THREAD_COUNT] =\n{\n");
	threads_count = 1;
	for( int i = 0; i < mod->modules_count; i++ )
	{
		int thread_index = threads_count;
		if( strcmp(mod->modules[i].name, "system") == 0 ) { thread_index = 0; }
		else { threads_count++; }
		fprintf(context->file, "\t[%i] = &thread%i_name,\n", thread_index, thread_index);
	}
	fprintf(context->file, "};\n");
}
