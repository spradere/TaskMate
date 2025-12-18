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

void writeInclude(const modules_database_t *data_base, const int type, const char *file_include_name,
				  const target_t *target)
{
	// open file
	FILE *file_include = fopen(file_include_name, "w");
	if( file_include == NULL )
	{
		msgError("creating file <%s>", file_include_name);
		exit(1);
	}

	// generate multiple include guard name
	char cmd[256];
	char guard_name[256];

	snprintf(cmd, sizeof(cmd), "printf \"%s\" | sed 's#.*/##' | tr a-z A-Z | sed 's/[^A-Z0-9_]/_/g'",
			 file_include_name);

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

	msgInfo("generated guard name %s", guard_name);

	// write code
	printLicenceHeader(file_include);
	printWarningHeader(file_include);

	if( type == INCLUDE_HAL_TARGET_PART )
	{
		fprintf(file_include, "// info : build target is %s/%s/%s\n\n", target->arch_name, target->mcu_name,
				target->board_name);

		fprintf(file_include, "#ifndef %s\n", guard_name);
		fprintf(file_include, "#define %s\n\n", guard_name);

		fprintf(file_include, "#include \"hal/hal_target_type.h\"\n");
		fprintf(file_include, "extern const target_info_t target;\n\n");

		fprintf(file_include, "#include \"hal/arch/%s/arch_define.h\"\n", target->arch_name);
		fprintf(file_include, "#include \"hal/mcu/%s/mcu_define.h\"\n", target->mcu_name);
		fprintf(file_include, "#include \"hal/board/%s/board_define.h\"\n", target->board_name);
		fprintf(file_include, "\n#endif\n");
	}

	if( type == INCLUDE_HAL_TARGET_NAME_PART)
	{
		fprintf(file_include, "#include \"hal/hal_target_type.h\"\n\n");
		fprintf(file_include, "const target_info_t target =\n");
		fprintf(file_include, "{\n");
		fprintf(file_include, ".arch = \"%s\",\n",target->arch_name);
		fprintf(file_include, ".mcu = \"%s\",\n",target->mcu_name);
		fprintf(file_include, ".board = \"%s\"\n\n",target->board_name);
		fprintf(file_include, "};");
	}

	if( type == INCLUDE_HAL_SYSTEM_CRITICAL_PART )
	{
		fprintf(file_include, "#ifndef %s\n", guard_name);
		fprintf(file_include, "#define %s\n\n", guard_name);

		fprintf(file_include, "#ifndef AUTOINCLUDE_HAL_SYSTEM_CRITICAL_ALLOWED\n");
		fprintf(file_include, "\t#error \"autoInclude system critical not allowed\"\n");
		fprintf(file_include, "#endif\n\n");

		fprintf(file_include, "#include \"hal/arch/%s/hal_stack.h\"\n", target->arch_name);
		fprintf(file_include, "#include \"hal/arch/%s/hal_context.h\"\n", target->arch_name);

		fprintf(file_include, "\n#endif\n");
	}

	if( type == INCLUDE_THREAD_PART )
	{
		fprintf(file_include, "#ifndef %s\n", guard_name);
		fprintf(file_include, "#define %s\n\n", guard_name);

		const module_type_t *mod = &data_base->modules_type[MODULES_SERVICES_ID];

		for( int i = 0; i < mod->modules_count; i++ )
		{
			fprintf(file_include, "#include \"services/%s.h\"\n", mod->modules[i].name);
		}
		fprintf(file_include, "\n");

		mod = &data_base->modules_type[MODULES_TASKS_ID];

		for( int i = 0; i < mod->modules_count; i++ )
		{
			fprintf(file_include, "#include \"tasks/%s.h\"\n", mod->modules[i].name);
		}
		fprintf(file_include, "\n#endif\n");
	}

	fclose(file_include);
}
