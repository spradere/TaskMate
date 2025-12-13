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
 * @file global_error.c
 * @brief write all error in one file
 *
 */

 # include "globalError.h"

void globalError(const char *file_name)
{
	msgInfo("open error file :");
	printf("\t <%s>\n\n",file_name);

	// open file
	FILE *file_err_in = fopen(file_name, "r");
	if( file_err_in == NULL )
	{
		msgError("open error file :");
		printf("\t <%s>\n", file_name);
		exit(1);
	}

	FILE *file_err_out = fopen("src/sysCall/error_new.h", "w");
	if( file_err_out == NULL )
	{
		msgError("open error file : scr/sysCall/error_new.h");
		exit(1);
	}


	fclose(file_err_in);
	fclose(file_err_out);
}
