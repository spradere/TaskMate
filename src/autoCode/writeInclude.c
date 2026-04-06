/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file writeInclude.c
 * @brief autoCode writeInclude implementation.
 *
 */

#include "writeInclude.h"

#include "fileUtility.h"
#include "tokenizer.h"

void writeInclude(const modules_database_t *data_base, include_type_t type, const char *file_name,
				  const options_list_t *auto_options)
{
	// open file
	msgInfo("generate <%s>", file_name);
	file_t file_include;
	fileInit(&file_include);
	file_include.name = (char *)file_name;
	fileOpen(&file_include, "r", FILE_CREATE, __FILE__, __LINE__);

	file_t file_tmp;
	fileInit(&file_tmp);
	fileMakeTmp(file_name, &file_tmp, __FILE__, __LINE__);

	// generate multiple include guard name
	char guard_name[BYTE_INDEX];
	generateGuardName(file_name, guard_name);

	// write code
	printLicenceHeader(file_tmp.stream);
	printWarningHeader(file_tmp.stream);

	fprintf(file_tmp.stream, "/**\n");
	fprintf(file_tmp.stream, " * @file %s\n", file_name);
	fprintf(file_tmp.stream, " * @brief autoCode generated file\n");
	fprintf(file_tmp.stream, " */\n");

	fprintf(file_tmp.stream, "#ifndef %s\n", guard_name);
	fprintf(file_tmp.stream, "#define %s\n\n", guard_name);

	printClangFormatOff(file_tmp.stream);

	switch( type )
	{
		/*case WI_HAL_DEFINE:

			fprintf(file_tmp.stream, "// target define\n");
			fprintf(file_tmp.stream,
					"#include \"hal/arch/%s/arch_define.h\"\n",
					auto_options->arch_name);
			fprintf(
				file_tmp.stream, "#include \"hal/mcu/%s/mcu_define.h\"\n", auto_options->mcu_name);
			fprintf(file_tmp.stream,
					"#include \"hal/board/%s/board_define.h\"\n\n",
					auto_options->board_name);
			break;*/

		case WI_HAL_INIT:

			fprintf(file_tmp.stream, "// target init\n");
			fprintf(file_tmp.stream,
					"#include \"hal/arch/%s/hal_archInit.h\"\n",
					auto_options->arch_name);
			fprintf(
				file_tmp.stream, "#include \"hal/mcu/%s/hal_mcuInit.h\"\n", auto_options->mcu_name);
			fprintf(file_tmp.stream,
					"#include \"hal/board/%s/hal_boardInit.h\"\n\n",
					auto_options->board_name);
			break;

		/*case WI_MOD_LIST:
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
			fprintf(file_tmp.stream, "\n");

			mod = &data_base->modules_type[MOD_DRIVERS_ID];

			for( int i = 0; i < mod->modules_count; i++ )
			{
				fprintf(file_tmp.stream, "#include \"hal/public/%s.h\"\n", mod->modules[i].name);
			}
		}
		break;*/

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
