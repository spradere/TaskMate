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
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/parseTag.h"
#include "utility/autoCode_src/tokenizer.h"

void parseTag(module_t *modules, char *name_src)
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
	name_tmp = malloc(strlen(name_src)+strlen(".tmp")+1);
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

	while( fgets(tok.line, TOKEN_LINE_SIZE_MAX, file_src) )
	{
		file_line_number++;
		tokenizer(&tok);

		if( !(strcmp(tok.tokens[0], "//")) && !(strcmp(tok.tokens[1], "[tag]")) )
		{
			if( tok.count != 4 )
			{
				msgError("token count != 4 tok.line :");
				printf("\t [%s:%i] %s\n\n", name_src, file_line_number, tok.line);
				break;
			}

			msgInfo("found tag :");
			printf("\t %s %s\n", tok.tokens[2], tok.tokens[3]);

			fprintf(file_tmp, "%s", tok.line);
			tag_section = 1;

			if( (strcmp(tok.tokens[2], "threads") == 0) && (strcmp(tok.tokens[3], "init") == 0) )
			{
				writeThreadsInit(modules, file_tmp);
			}

			if( (strcmp(tok.tokens[2], "drivers") == 0) && (strcmp(tok.tokens[3], "init") == 0) )
			{
				writeDriversInit(modules, file_tmp);
			}
		}

		if( !(strcmp(tok.tokens[0], "//")) && !(strcmp(tok.tokens[1], "[/tag]")) )
		{
			msgInfo("end tag");
			tag_section = 0;
		}

		if( tag_section == 0 ) { fprintf(file_tmp, "%s", tok.line); }
	}

	if( tag_section == 1 )
	{
		msgError("missing end tag [/tag] at end of file");
		printf("\t [%s]\n\n", name_src);
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

void writeThreadsInit(module_t *modules, FILE *file)
{
	int threads_count = 0;

	for( int i = 0; i < modules->services_count; i++ )
	{
		fprintf(file, "\tthreadCreate(%s, %i);\n\n", modules->services[i].name, i);

		fprintf(file, "\tconst char* thread%i_name = \"%s\";\n", i, modules->services[i].name);
		fprintf(file, "\tmodules.threads[%i].id = %i;\n", i, i);
		fprintf(file, "\tmodules.threads[%i].name = (uint8_t *)thread%i_name;\n", i, i);
		fprintf(file, "\tmodules.threads[%i].setStatus = %sSetStatus;\n", i, modules->services[i].name);
		fprintf(file, "\tmodules.threads[%i].getStatus = %sGetStatus;\n", i, modules->services[i].name);
		fprintf(file, "\t(*modules.threads[%i].setStatus)(%i);\n\n", i, modules->services[i].status);
	}

	for( int i = 0; i < modules->tasks_count; i++ )
	{
		int thread_count;
		thread_count = i + modules->services_count;

		fprintf(file, "\tthreadCreate(%s, %i);\n\n", modules->tasks[i].name, thread_count);

		fprintf(file, "\tconst char* thread%i_name = \"%s\";\n", thread_count, modules->tasks[i].name);
		fprintf(file, "\tmodules.threads[%i].id = %i;\n", thread_count, thread_count);
		fprintf(file, "\tmodules.threads[%i].name = (uint8_t *)thread%i_name;\n", thread_count, thread_count);
		fprintf(file, "\tmodules.threads[%i].setStatus = %sSetStatus;\n", thread_count, modules->tasks[i].name);
		fprintf(file, "\tmodules.threads[%i].getStatus = %sGetStatus;\n", thread_count, modules->tasks[i].name);
		fprintf(file, "\t(*modules.threads[%i].setStatus)(%i);\n\n", thread_count, modules->tasks[i].status);
	}
}

void writeDriversInit(module_t *modules, FILE *file)
{
	for( int i = 0; i < modules->drivers_count; i++ )
	{
		fprintf(file, "\tconst char* driver%i_name = \"%s\";\n", i, modules->drivers[i].name);

		fprintf(file, "\tmodules.drivers[%i]=(driver_item_t)\n", i);
		fprintf(file, "\t{\n");
		fprintf(file, "\t\t.id = %i,\n", i);
		fprintf(file, "\t\t.name = (uint8_t *)driver%i_name,\n", i);
		fprintf(file, "\t\t.setStatus = %sSetStatus,\n", modules->drivers[i].name);
		fprintf(file, "\t\t.getStatus = %sGetStatus,\n", modules->drivers[i].name);
		fprintf(file, "\t\t.init = %sInit,\n", modules->drivers[i].name);
		fprintf(file, "\t\t.start = %sStart,\n", modules->drivers[i].name);
		fprintf(file, "\t\t.stop = %sStop\n", modules->drivers[i].name);
		fprintf(file, "\t};\n");
		fprintf(file, "\t(*modules.drivers[%i].setStatus)(%i);\n\n", i, modules->drivers[i].status);
	}
}
