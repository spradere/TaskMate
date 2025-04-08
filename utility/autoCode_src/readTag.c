/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file readTag.c
 * @brief read tag witre code in file implemetation
 *
 * @todo find solution to sting copy
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/readTag.h"
#include "utility/autoCode_src/tokenizer.h"

void readTag(list_table_t *table, char *line, char **argv)
{
	// open source and tmp file
	FILE *file_src = fopen(FILE_SOURCE, "r");
	if (file_src == NULL)
	{
		ERRMSG("opening file");
		printf("\t <%s>\n", FILE_SOURCE);
		exit(1);
	}

	FILE *file_tmp = fopen(FILE_TEMP, "w");
	if (file_tmp == NULL)
	{
		ERRMSG("creating file");
		printf("\t <%s>\n", FILE_TEMP);
		exit(1);
	}

	// read form source
	int tag_section = 0;
	int file_line_number = 0;
	int i, arg_count;

	while (fgets(line, LINE_SIZE_MAX, file_src))
	{
		file_line_number++;
		arg_count = tokenizer(line, argv);

		if (!(strcmp(argv[0], "//")) && !(strcmp(argv[1], "[tag]")))
		{
			if (arg_count != 4)
			{
				ERRMSG("arg count != 4 line :");
				printf("\t %s:%i\n", FILE_SOURCE, file_line_number);
				break;
			}

			printf("found tag <%s> <%s> ... ", argv[2], argv[3]);
			fprintf(file_tmp, "%s", line);
			tag_section = 1;

			if (!(strcmp(argv[2], "task"))) // task tag
			{
				if (!(strcmp(argv[3], "init"))) // task init
				{
					for (i = 0; i < table->task_count; i++)
					{
						fprintf(file_tmp, "\ttaskCreate(%s, %i);\n\n",
						 table->task_list[i]->name, i);
						 
						fprintf(file_tmp, "\tconst char* task%i_name = \"%s\";\n",
							i, table->task_list[i]->name);
						fprintf(file_tmp, "\ttask_table[%i].task_name = (uint8_t *)task%i_name;\n",
							i, i);
						fprintf(file_tmp, "\ttask_table[%i].setStatus = %sSetStatus;\n",
							i, table->task_list[i]->name);
						fprintf(file_tmp, "\ttask_table[%i].getStatus = %sGetStatus;\n",
							i, table->task_list[i]->name);
						fprintf(file_tmp, "\t(*task_table[%i].setStatus)(%i);\n\n",
							i, table->task_list[i]->status);

					}
				}
			}

			if (!(strcmp(argv[2], "driver"))) // driver tag
			{
				if (!(strcmp(argv[3], "init"))) // driver init
				{
					for (i = 0; i < table->driver_count; i++)
					{
						fprintf(file_tmp, "\tconst char* driver%i_name = \"%s\";\n",
							i, table->driver_list[i]->name);

						fprintf(file_tmp, "\tdriver_table[%i]=(driver_table_t)\n", i);
						fprintf(file_tmp, "\t{\n");
						fprintf(file_tmp, "\t\t.driver_id = %i,\n", i);
						fprintf(file_tmp, "\t\t.driver_name = (uint8_t *)driver%i_name,\n", i);
						fprintf(file_tmp, "\t\t.setStatus = %sSetStatus,\n", table->driver_list[i]->name);
						fprintf(file_tmp, "\t\t.getStatus = %sGetStatus,\n", table->driver_list[i]->name);
						fprintf(file_tmp, "\t\t.init = %sInit,\n", table->driver_list[i]->name);
						fprintf(file_tmp, "\t\t.start = %sStart,\n", table->driver_list[i]->name);
						fprintf(file_tmp, "\t\t.stop = %sStop\n", table->driver_list[i]->name);
						fprintf(file_tmp, "\t};\n");
						fprintf(file_tmp, "\t(*driver_table[%i].setStatus)(%i);\n\n",
							i, table->driver_list[i]->status);
					}
				}
			}
		}

		if (!(strcmp(argv[0], "//")) && !(strcmp(argv[1], "[/tag]")))
			{printf("end tag\n"); tag_section = 0;}

		if (tag_section == 0) {fprintf(file_tmp, "%s", line);}
	}

	if(tag_section == 1) {ERRMSG("missing end tag [/tag] at end of file");}

	// Replace original file with the modified version
	if (remove(FILE_SOURCE) != 0 || rename(FILE_TEMP, FILE_SOURCE) != 0)
	{
		ERRMSG("replacing initSys.c");
		exit(2);
	}

	fclose(file_src);
	fclose(file_tmp);
}
