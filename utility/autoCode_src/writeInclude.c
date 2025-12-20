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
 * @brief write include code implemetation
 *
 */

#include "utility/autoCode_src/writeInclude.h"
#include "utility/autoCode_src/fileUtility.h"

void writeInclude(const modules_database_t *data_base, const int type, const char *file_name,
				  const target_t *target)
{
	// open file
	msgInfo("generate include statements in <%s>", file_name);
	file_t file_include;
	fileInit(&file_include);
	file_include.name = (char*)file_name;
	fileOpen(&file_include, "r", __FILE__);

	file_t file_tmp;
	fileInit(&file_tmp);
	fileMakeTmp(file_name, &file_tmp, __FILE__);

	/*FILE *file_include = fopen(file_include_name, "w");
	if( file_include == NULL )
	{
		msgError("creating file <%s>", file_include_name);
		exit(1);
	}*/

	// generate multiple include guard name
	char cmd[256];
	char guard_name[256];

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

	if( type == INCLUDE_HAL_TARGET_PART )
	{
		fprintf(file_tmp.stream, "// info : build target is %s/%s/%s\n\n", target->arch_name, target->mcu_name,
				target->board_name);

		fprintf(file_tmp.stream, "#ifndef %s\n", guard_name);
		fprintf(file_tmp.stream, "#define %s\n\n", guard_name);

		fprintf(file_tmp.stream, "#include \"hal/hal_target_type.h\"\n");
		fprintf(file_tmp.stream, "extern const target_info_t target;\n\n");

		fprintf(file_tmp.stream, "#include \"hal/arch/%s/arch_define.h\"\n", target->arch_name);
		fprintf(file_tmp.stream, "#include \"hal/mcu/%s/mcu_define.h\"\n", target->mcu_name);
		fprintf(file_tmp.stream, "#include \"hal/board/%s/board_define.h\"\n", target->board_name);
		fprintf(file_tmp.stream, "\n#endif\n");
	}

	if( type == INCLUDE_HAL_TARGET_NAME_PART)
	{
		fprintf(file_tmp.stream, "#include \"hal/hal_target_type.h\"\n\n");
		fprintf(file_tmp.stream, "const target_info_t target =\n");
		fprintf(file_tmp.stream, "{\n");
		fprintf(file_tmp.stream, ".arch = \"%s\",\n",target->arch_name);
		fprintf(file_tmp.stream, ".mcu = \"%s\",\n",target->mcu_name);
		fprintf(file_tmp.stream, ".board = \"%s\"\n\n",target->board_name);
		fprintf(file_tmp.stream, "};");
	}

	if( type == INCLUDE_HAL_SYSTEM_CRITICAL_PART )
	{
		fprintf(file_tmp.stream, "#ifndef %s\n", guard_name);
		fprintf(file_tmp.stream, "#define %s\n\n", guard_name);

		fprintf(file_tmp.stream, "#ifndef AUTOINCLUDE_HAL_SYSTEM_CRITICAL_ALLOWED\n");
		fprintf(file_tmp.stream, "\t#error \"autoInclude system critical not allowed\"\n");
		fprintf(file_tmp.stream, "#endif\n\n");

		fprintf(file_tmp.stream, "#include \"hal/arch/%s/hal_stack.h\"\n", target->arch_name);
		fprintf(file_tmp.stream, "#include \"hal/arch/%s/hal_context.h\"\n", target->arch_name);

		fprintf(file_tmp.stream, "\n#endif\n");
	}

	if( type == INCLUDE_THREAD_PART )
	{
		fprintf(file_tmp.stream, "#ifndef %s\n", guard_name);
		fprintf(file_tmp.stream, "#define %s\n\n", guard_name);

		const module_type_t *mod = &data_base->modules_type[MODULES_SERVICES_ID];

		for( int i = 0; i < mod->modules_count; i++ )
		{
			fprintf(file_tmp.stream, "#include \"services/%s.h\"\n", mod->modules[i].name);
		}
		fprintf(file_tmp.stream, "\n");

		mod = &data_base->modules_type[MODULES_TASKS_ID];

		for( int i = 0; i < mod->modules_count; i++ )
		{
			fprintf(file_tmp.stream, "#include \"tasks/%s.h\"\n", mod->modules[i].name);
		}
		fprintf(file_tmp.stream, "\n#endif\n");
	}

	fileCmpReplace(&file_include, &file_tmp);

	fileClose(&file_include, __FILE__);
	fileClose(&file_tmp, __FILE__);

}
