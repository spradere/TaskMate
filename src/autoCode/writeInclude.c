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
 * @file writeInclude.c
 * @brief write include files
 *
 */

#include "writeInclude.h"

#include "fileUtility.h"

void writeInclude(const modules_database_t *data_base, include_type_t type, const char *file_name,
				  const auto_options_t *auto_options)
{
	// open file
	msgInfo("generate include statements in <%s>", file_name);
	file_t file_include;
	fileInit(&file_include);
	file_include.name = (char *)file_name;
	fileOpen(&file_include, "r", __FILE__, __LINE__);

	file_t file_tmp;
	fileInit(&file_tmp);
	fileMakeTmp(file_name, &file_tmp, __FILE__, __LINE__);

	// generate multiple include guard name
	char cmd[BYTE_INDEX];
	char guard_name[BYTE_INDEX];

	snprintf(cmd, sizeof(cmd), "printf \"%s\" | sed 's#.*/##' | tr a-z A-Z | sed 's/[^A-Z0-9_]/_/g'",
			 file_name);

	FILE *p_cmd = popen(cmd, "r");
	if( !p_cmd )
	{
		msgError("failed open command pipe");
		exit(1);
	}

	if( !fgets(guard_name, sizeof(guard_name), p_cmd) )
	{
		pclose(p_cmd);
		msgError("failed get string form command pipe");
		exit(1);
	}

	// write code
	printLicenceHeader(file_tmp.stream);
	printWarningHeader(file_tmp.stream);

	fprintf(file_tmp.stream, "#ifndef %s\n", guard_name);
	fprintf(file_tmp.stream, "#define %s\n\n", guard_name);

	printClangFormatOff(file_tmp.stream);

	switch( type )
	{
		case INCLUDE_HAL_DEFINE:

			fprintf(file_tmp.stream, "// target define\n");
			fprintf(file_tmp.stream, "#include \"hal/arch/%s/arch_define.h\"\n", auto_options->arch_name);
			fprintf(file_tmp.stream, "#include \"hal/mcu/%s/mcu_define.h\"\n", auto_options->mcu_name);
			fprintf(file_tmp.stream, "#include \"hal/board/%s/board_define.h\"\n\n", auto_options->board_name);
			break;

		case INCLUDE_HAL_INIT:

			fprintf(file_tmp.stream, "// target init\n");
			fprintf(file_tmp.stream, "#include \"hal/arch/%s/hal_archInit.h\"\n", auto_options->arch_name);
			fprintf(file_tmp.stream, "#include \"hal/mcu/%s/hal_mcuInit.h\"\n", auto_options->mcu_name);
			fprintf(file_tmp.stream, "#include \"hal/board/%s/hal_boardInit.h\"\n\n", auto_options->board_name);
			break;

		case INCLUDE_HAL_USER_PART:
		case INCLUDE_HAL_SYSTEM_PART:
		{
			file_t file_hal;
			fileInit(&file_hal);
			if( type == INCLUDE_HAL_USER_PART ) { file_hal.name = "build/files_hal_user"; }
			if( type == INCLUDE_HAL_SYSTEM_PART ) { file_hal.name = "build/files_hal_system"; }
			fileOpen(&file_hal, "r", __FILE__, __LINE__);

			if( type == INCLUDE_HAL_USER_PART )
			{
				fprintf(file_tmp.stream, "// autoInclude hal user headers\n");
			}
			if( type == INCLUDE_HAL_SYSTEM_PART )
			{
				fprintf(file_tmp.stream, "// autoInclude hal system headers\n");
			}

			int ret;
			do
			{
				ret = fileGetToken(&file_hal);
				if( strlen(file_hal.token) != 0 )
				{
					fprintf(file_tmp.stream, "#include \"%s\"\n", file_hal.token);
				}
			} while( ret != 0 );
		}

		break;

		case INCLUDE_THREAD_LIST:
		{
			const module_type_t *mod = &data_base->modules_type[MOD_SERVICES_ID];

			for( int i = 0; i < mod->modules_count; i++ )
			{
				fprintf(file_tmp.stream, "#include \"services/%s.h\"\n", mod->modules[i].name);
			}
			fprintf(file_tmp.stream, "\n");

			mod = &data_base->modules_type[MOD_TASKS_ID];

			for( int i = 0; i < mod->modules_count; i++ )
			{
				fprintf(file_tmp.stream, "#include \"tasks/%s.h\"\n", mod->modules[i].name);
			}
		}
		break;

		default:
			msgError("unrecognised type %i", type);
			exit(1);
	}

	printClangFormatOn(file_tmp.stream);
	fprintf(file_tmp.stream, "\n#endif\n");

	fileCmpReplace(&file_include, &file_tmp);

	fileClose(&file_include, __FILE__, __LINE__);
	fileClose(&file_tmp, __FILE__, __LINE__);
}
