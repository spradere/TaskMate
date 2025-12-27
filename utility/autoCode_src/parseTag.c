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

#include "parseTag.h"
#include "tokenizer.h"
#include "fileUtility.h"

static void writeTaget(const target_t *target, FILE *file);
static void writeDriversInit(modules_database_t *data_base, FILE *file);
static void writeThreadsInit(modules_database_t *data_base, FILE *file);
static void writeRunLevelsInit(modules_database_t *data_base, FILE *file);
static void writeErrorCatalog(const error_catalog_t *errors, FILE *file);

void parseTag(modules_database_t *data_base, const char *file_name, const error_catalog_t *errors,
			  const target_t *target)
{
	// open source and tmp file
	msgInfo("open <%s> for parsing tag section", file_name);

	file_t file_src;
	fileInit(&file_src);
	file_src.name = (char *)file_name;
	fileOpen(&file_src, "r", __FILE__);

	file_t file_tmp;
	fileInit(&file_tmp);
	fileMakeTmp(file_src.name, &file_tmp, __FILE__);

	// read form source
	int tag_section = 0;
	int file_line_number = 0;
	tokenizer_t tok;

	while( fgets(tok.line, TOKEN_LINE_SIZE_MAX, file_src.stream) )
	{
		file_line_number++;
		tokenizer(&tok);

		if( !(strcmp(tok.tokens[0], "//")) && !(strcmp(tok.tokens[1], "[autoCode_tag]")) )
		{
			if( tok.count != 4 )
			{
				msgError("token count != 4 tok.line [%s:%i] %s", file_src.name, file_line_number, tok.line);
				break;
			}

			if( tag_section == 1 )
			{
				msgError("Start new tag section without previous end tag [/tag] [%s:%i] %s", file_src.name,
						 file_line_number, tok.line);
				break;
			}

			msgInfo("found tag %s %s", tok.tokens[2], tok.tokens[3]);

			fprintf(file_tmp.stream, "%s", tok.line);
			tag_section = 1;

			if( (strcmp(tok.tokens[2], "threads") == 0) && (strcmp(tok.tokens[3], "init") == 0) )
			{
				writeThreadsInit(data_base, file_tmp.stream);
			}

			if( (strcmp(tok.tokens[2], "drivers") == 0) && (strcmp(tok.tokens[3], "init") == 0) )
			{
				writeDriversInit(data_base, file_tmp.stream);
			}

			if( (strcmp(tok.tokens[2], "run") == 0) && (strcmp(tok.tokens[3], "levels") == 0) )
			{
				writeRunLevelsInit(data_base, file_tmp.stream);
			}

			if( (strcmp(tok.tokens[2], "error") == 0) && (strcmp(tok.tokens[3], "catalog") == 0) )
			{
				writeErrorCatalog(errors, file_tmp.stream);
			}

			if( (strcmp(tok.tokens[2], "target") == 0) && (strcmp(tok.tokens[3], "name") == 0) )
			{
				writeTaget(target, file_tmp.stream);
			}
		}

		if( !(strcmp(tok.tokens[0], "//")) && !(strcmp(tok.tokens[1], "[/tag]")) )
		{
			msgInfo("end tag");
			tag_section = 0;
		}

		if( tag_section == 0 ) { fprintf(file_tmp.stream, "%s", tok.line); }
	}

	if( tag_section == 1 )
	{
		msgError("missing end tag [/tag] [%s:%i]", file_src.name, file_line_number);
		exit(1);
	}

	fileCmpReplace(&file_src, &file_tmp);

	fileClose(&file_src, __FILE__);
	fileClose(&file_tmp, __FILE__);
}

static void writeTaget(const target_t *target, FILE *file)
{
	// write target name
	//fprintf(file, "#include \"hal/hal_target_type.h\"\n\n");
	fprintf(file, "const target_info_t target_info =\n");
	fprintf(file, "{\n");
	fprintf(file, ".arch = \"%s\",\n", target->arch_name);
	fprintf(file, ".mcu = \"%s\",\n", target->mcu_name);
	fprintf(file, ".board = \"%s\"\n", target->board_name);
	fprintf(file, "};\n");
}

static void writeThreadsInit(modules_database_t *data_base, FILE *file)
{
	int threads_count = 0;
	const module_type_t *mod;
	int type;

	fprintf(file, "\tmodule_item_thread_t *mod_t;\n");

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

			fprintf(file, "\n\tmod_t = moduleThreadGetPointer(%i);\n", threads_count);

			fprintf(file,
					"\n\thal_threadContextInit(%s, &(mod_t->stack_pointer), "
					"&(mod_t->stack[THREAD_STACK_SIZE - 1 ]));\n",
					mod->modules[i].name);

			fprintf(file, "\tmod_t->real_time_counter = 0;\n");
			fprintf(file, "\tconst char *thread%i_name = \"%s\";\n", threads_count, mod->modules[i].name);

			fprintf(file, "\tmod_t->name = thread%i_name;\n", threads_count);

			fprintf(file, "\tmod_t->status = %i;\n", mod->modules[i].status | type);

			fprintf(file, "\tmod_t->main = %s;\n", mod->modules[i].name);

			threads_count++;
		}
	}
}

static void writeDriversInit(modules_database_t *data_base, FILE *file)
{
	const module_type_t *mod = &data_base->modules_type[MODULES_DRIVERS_ID];

	fprintf(file, "\tmodule_item_driver_t *mod_d;\n");

	for( int i = 0; i < mod->modules_count; i++ )
	{
		fprintf(file, "\n\tmod_d = moduleDriverGetPointer(%i);\n", i);

		fprintf(file, "\tconst char *driver%i_name = \"%s\";\n", i, mod->modules[i].name);

		fprintf(file, "\t*(mod_d) = (module_item_driver_t)\n");
		fprintf(file, "\t{\n");
		fprintf(file, "\t\t.name = driver%i_name,\n", i);
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

static void writeErrorCatalog(const error_catalog_t *errors, FILE *file)
{
	fprintf(file, "const error_item_t error_catalog[] = \n{\n");

	for( int i = 0; i < errors->error_count - 1; i++ )
	{
		fprintf(file, "\t{%s, %i},\n", errors->catalog[i].message, errors->catalog[i].critical);
	}
	fprintf(file, "\t{%s, %i}\n", errors->catalog[errors->error_count - 1].message,
			errors->catalog[errors->error_count - 1].critical);
	fprintf(file, "};\n");
}
