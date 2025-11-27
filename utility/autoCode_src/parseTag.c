/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file parseTagDriversInit.c
 * @brief read tag witre code in file implemetation
 *
 */

#include "utility/autoCode_src/parseTag.h"
#include "utility/autoCode_src/tokenizer.h"

void parseTag(modules_database_t *data_base, const char *name_src)
{
	// open source and tmp file
	FILE *file_src = fopen(name_src, "r");
	if( file_src == NULL )
	{
		msgError("opening file");
		printf("\t <%s>\n", name_src);
		exit(1);
	}

	char *name_tmp;
	name_tmp = malloc(strlen(name_src) + strlen(".tmp") + 1);
	if( name_tmp == NULL )
	{
		msgError("malloc name_tmp");
		exit(0);
	}
	sprintf(name_tmp, "%s.tmp", name_src);

	FILE *file_tmp = fopen(name_tmp, "w");
	if( file_tmp == NULL )
	{
		msgError("creating file");
		printf("\t <%s>\n", name_tmp);
		exit(1);
	}

	// read form source
	int tag_section = 0;
	int file_line_number = 0;
	tokenizer_t tok;
	printf("\n");

	while( fgets(tok.line, TOKEN_LINE_SIZE_MAX, file_src) )
	{
		file_line_number++;
		tokenizer(&tok);

		if( !(strcmp(tok.tokens[0], "//")) && !(strcmp(tok.tokens[1], "[autoCode_tag]")) )
		{
			if( tok.count != 4 )
			{
				msgError("token count != 4 tok.line :");
				printf("\t [%s:%i] %s\n\n", name_src, file_line_number, tok.line);
				break;
			}

			if( tag_section == 1 )
			{
				msgError("Start new tag section without previous end tag [/tag]");
				printf("\t [%s:%i] %s\n\n", name_src, file_line_number, tok.line);
				break;
			}

			msgInfo("found tag :");
			printf("\t %s %s\n", tok.tokens[2], tok.tokens[3]);

			fprintf(file_tmp, "%s", tok.line);
			tag_section = 1;

			if( (strcmp(tok.tokens[2], "threads") == 0) && (strcmp(tok.tokens[3], "init") == 0) )
			{
				writeThreadsInit(data_base, file_tmp);
			}

			if( (strcmp(tok.tokens[2], "drivers") == 0) && (strcmp(tok.tokens[3], "init") == 0) )
			{
				writeDriversInit(data_base, file_tmp);
			}

			if( (strcmp(tok.tokens[2], "run") == 0) && (strcmp(tok.tokens[3], "levels") == 0) )
			{
				writeRunLevelsInit(data_base, file_tmp);
			}
		}

		if( !(strcmp(tok.tokens[0], "//")) && !(strcmp(tok.tokens[1], "[/tag]")) )
		{
			msgInfo("end tag\n");
			tag_section = 0;
		}

		if( tag_section == 0 ) { fprintf(file_tmp, "%s", tok.line); }
	}

	if( tag_section == 1 )
	{
		msgError("missing end tag [/tag]");
		printf("\t [%s:%i]\n\n", name_src, file_line_number);
		exit(0);
	}

	// Replace original file with the modified version
	if( (remove(name_src) != 0) || (rename(name_tmp, name_src) != 0) )
	{
		msgError("replacing tmp / src");
		exit(2);
	}

	fclose(file_src);
	fclose(file_tmp);
	free(name_tmp);
}

static void writeThreadsInit(modules_database_t *data_base, FILE *file)
{
	int threads_count = 0;
	const module_type_t *mod;
	int type;

	fprintf(file,"\tmodule_item_thread_t *mod_t;\n");

	for( int j = 0; j < 2; j++ )
	{
		if( j == 0 )
		{
			mod = &data_base->modules_type[MODULES_SERVICES_ID];
			type = (1 << MODULES_THREAD_TYPE_SYSTEM);
		}
		if( j == 1 )
		{
			mod = &data_base->modules_type[MODULES_TASKS_ID];
			type = (1 << MODULES_THREAD_TYPE_USER);
		}

		for( int i = 0; i < mod->modules_count; i++ )
		{

			fprintf(file,"\n\tmod_t = moduleThreadGetPointer(%i);\n", threads_count);

			fprintf(file,
					"\n\thal_threadContextInit(%s, &(mod_t->stack_pointer), "
					"&(mod_t->stack[THREAD_STACK_SIZE -1 ]) );\n",
					mod->modules[i].name );

			fprintf(file, "\tmod_t->real_time_counter = 0;\n");
			fprintf(file, "\tconst char *thread%i_name = \"%s\";\n", threads_count, mod->modules[i].name);

			fprintf(file, "\tmod_t->name = (uint8_t *)thread%i_name;\n", threads_count);

			fprintf(file, "\tmod_t->status = %i;\n", mod->modules[i].status | type);

			fprintf(file, "\tmod_t->main = %s;\n", mod->modules[i].name);

			threads_count++;
		}
	}
}

static void writeDriversInit(modules_database_t *data_base, FILE *file)
{
	const module_type_t *mod = &data_base->modules_type[MODULES_DRIVERS_ID];

	fprintf(file,"\tmodule_item_driver_t *mod_d;\n");

	for( int i = 0; i < mod->modules_count; i++ )
	{
		fprintf(file,"\n\tmod_d = moduleDriverGetPointer(%i);\n", i);

		fprintf(file, "\tconst char *driver%i_name = \"%s\";\n", i, mod->modules[i].name);

		fprintf(file, "\t*(mod_d) = (module_item_driver_t)\n");
		fprintf(file, "\t{\n");
		fprintf(file, "\t\t.name = (uint8_t *)driver%i_name,\n", i);
		fprintf(file, "\t\t.status = %i,\n", mod->modules[i].status);
		fprintf(file, "\t\t.init = %sInit,\n", mod->modules[i].name);
		fprintf(file, "\t\t.start = %sStart,\n", mod->modules[i].name);
		fprintf(file, "\t\t.stop = %sStop\n", mod->modules[i].name);
		fprintf(file, "\t};\n");


	}
}

static void writeRunLevelsInit(modules_database_t *data_base, FILE *file)
{

	fprintf(file, "\tto_run = (run_levels_t){\n");

	for( int level = 0; level < RUN_LEVEL_COUNT; level++ )
	{

		// count thread (services + task) for run level
		int thread_count = 0;

		for( int i = 1; i <= level; i++ )
		{
			thread_count += data_base->run_level_module_count[MODULES_SERVICES_ID][i];
		}
		for( int i = 1; i <= level; i++ )
		{
			thread_count += data_base->run_level_module_count[MODULES_TASKS_ID][i];
		}

		data_base->threads_count[level] = thread_count;

		fprintf(file, "\t\t.level%i = {%i", level, data_base->threads_count[level]);

		// write threads list for run level
		int threads_count = 0;
		const module_type_t *mod;

		for( int j = 0; j < 2; j++ )
		{
			if( j == 0 ) { mod = &data_base->modules_type[MODULES_SERVICES_ID]; }
			if( j == 1 ) { mod = &data_base->modules_type[MODULES_TASKS_ID]; }

			for( int i = 0; i < mod->modules_count; i++ )
			{
				if( (mod->modules[i].status & RUN_LEVEL_MASK) <= level )
				{
					fprintf(file, ",%i", threads_count++);
				}
			}
		}
		fprintf(file, "},\n");
	}

	fprintf(file, "\t\t.levels = {to_run.level0, to_run.level1, to_run.level2, "
				  "to_run.level3, to_run.level4}\n");
	fprintf(file, "\t};\n");

	fprintf(file, "\tto_run.current=RUN_CORE;\n");
	fprintf(file, "\tto_run.next=RUN_CORE;\n");
}
