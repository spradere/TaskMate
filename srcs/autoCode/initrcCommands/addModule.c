/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file addModule.c
 * @brief init.rc addModule command implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "addModule.h"

#include <sys/stat.h>

/* =============================================================================
 * Declarations - Local types
 * ===========================================================================*/

typedef enum
{
	ADD_MODULE_OPTION_OK,
	ADD_MODULE_OPTION_INVALID_DATA
} add_module_option_result_t;

typedef enum
{
	OPTION_FORBIDDEN,
	OPTION_OPTIONAL,
	OPTION_REQUIRED,
	OPTION_CUMULATIVE
} option_cardinality_t;

typedef struct
{
	module_item_t *module;
	const char *source_path;
} add_module_option_context_t;

typedef add_module_option_result_t (*add_module_option_func_t)(
	const char *data, const add_module_option_context_t *context);

/* -----------------------------------------------
 * Module type dispatch table
 * ---------------------------------------------*/

#define ADD_MODULE_TYPE(X)                                                    \
	X(DRIVER, "driver", MOD_DRIVER_ID, 0)                                    \
	X(SERVICE, "service", MOD_THREAD_ID, THREAD_BIT_TYPE_SYS)                \
	X(TASK, "task", MOD_THREAD_ID, THREAD_BIT_TYPE_USER)

typedef enum
{
#define X(id, name, type, subtype) ADD_MODULE_##id,
	ADD_MODULE_TYPE(X)
#undef X
	ADD_MODULE_TYPE_COUNT
} add_module_type_t;

static const struct
{
	const char *name;
	unsigned char type;
	unsigned char subtype;
} module_types[] = {
#define X(id, name, type, subtype) {(name), (type), (subtype)},
	ADD_MODULE_TYPE(X)
#undef X
};

/* -----------------------------------------------
 * Module option dispatch table
 * ---------------------------------------------*/

static add_module_option_result_t optionRun(const char *data,
											 const add_module_option_context_t *context);
static add_module_option_result_t optionI2c(const char *data,
											 const add_module_option_context_t *context);
static add_module_option_result_t optionStack(const char *data,
											   const add_module_option_context_t *context);
static add_module_option_result_t optionSourceFile(
	const char *data, const add_module_option_context_t *context);
static add_module_option_result_t optionSourceDir(
	const char *data, const add_module_option_context_t *context);

#define ADD_MODULE_OPTION(X)                                                   \
	X(RUN, "-run", optionRun, OPTION_REQUIRED, OPTION_REQUIRED, OPTION_REQUIRED) \
	X(I2C, "-i2c", optionI2c, OPTION_OPTIONAL, OPTION_FORBIDDEN, OPTION_FORBIDDEN) \
	X(STACK, "-stack", optionStack, OPTION_FORBIDDEN, OPTION_REQUIRED,           \
	  OPTION_REQUIRED)                                                            \
	X(SOURCE_FILE, "-source_file", optionSourceFile, OPTION_CUMULATIVE,          \
	  OPTION_CUMULATIVE, OPTION_CUMULATIVE)                                       \
	X(SOURCE_DIR, "-source_dir", optionSourceDir, OPTION_CUMULATIVE,             \
	  OPTION_CUMULATIVE, OPTION_CUMULATIVE)

typedef enum
{
#define X(id, name, func, driver, service, task) ADD_MODULE_OPTION_##id,
	ADD_MODULE_OPTION(X)
#undef X
	ADD_MODULE_OPTION_COUNT
} add_module_option_t;

static const struct
{
	const char *name;
	add_module_option_func_t func;
	option_cardinality_t cardinality[ADD_MODULE_TYPE_COUNT];
} module_options[] = {
#define X(id, name, func, driver, service, task) {(name), (func), {(driver), (service), (task)}},
	ADD_MODULE_OPTION(X)
#undef X
};

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static add_module_option_result_t optionRun(const char *data,
											 const add_module_option_context_t *context)
{
	if( strcmp(data, "none") == 0 ) { context->module->status |= RL_RUN_NONE; }
	else if( strcmp(data, "core") == 0 ) { context->module->status |= RL_RUN_CORE; }
	else if( strcmp(data, "driver") == 0 ) { context->module->status |= RL_RUN_DRIVER; }
	else if( strcmp(data, "service") == 0 ) { context->module->status |= RL_RUN_SERVICE; }
	else if( strcmp(data, "user") == 0 ) { context->module->status |= RL_RUN_USER; }
	else { return ADD_MODULE_OPTION_INVALID_DATA; }

	return ADD_MODULE_OPTION_OK;
}

static add_module_option_result_t optionI2c(const char *data,
											 const add_module_option_context_t *context)
{
	char *end;
	const unsigned long address = strtoul(data, &end, 16);

	if( (data[0] == 0) || (*end != 0) || (address > MOD_I2C_ADDRESS_MAX) )
	{
		return ADD_MODULE_OPTION_INVALID_DATA;
	}

	context->module->address = (unsigned char)address;
	return ADD_MODULE_OPTION_OK;
}

static add_module_option_result_t optionStack(const char *data,
											   const add_module_option_context_t *context)
{
	char *end;
	const unsigned long stack_size = strtoul(data, &end, 10);

	if( (data[0] == 0) || (*end != 0) || (stack_size < AC_THREAD_STACK_SIZE_MIN) ||
		(stack_size > UINT16_MAX) )
	{
		return ADD_MODULE_OPTION_INVALID_DATA;
	}

	context->module->stack_size = (uint16_t)stack_size;
	return ADD_MODULE_OPTION_OK;
}

static add_module_option_result_t optionSource(const char *data,
											  const bool directory,
											  const add_module_option_context_t *context)
{
	char path[AC_BUFFER_SIZE];
	const int length = snprintf(path, sizeof(path), "%s/%s", context->source_path, data);

	if( (length < 0) || ((size_t)length >= sizeof(path)) )
	{
		return ADD_MODULE_OPTION_INVALID_DATA;
	}

	struct stat status;
	if( stat(path, &status) != 0 ) { return ADD_MODULE_OPTION_INVALID_DATA; }
	if( directory && !S_ISDIR(status.st_mode) ) { return ADD_MODULE_OPTION_INVALID_DATA; }
	if( !directory && !S_ISREG(status.st_mode) ) { return ADD_MODULE_OPTION_INVALID_DATA; }
	return ADD_MODULE_OPTION_OK;
}

static add_module_option_result_t optionSourceFile(
	const char *data, const add_module_option_context_t *context)
{
	return optionSource(data, false, context);
}

static add_module_option_result_t optionSourceDir(
	const char *data, const add_module_option_context_t *context)
{
	return optionSource(data, true, context);
}

static void moduleInit(module_item_t *module, const add_module_type_t module_type)
{
	module->status = 0;
	module->type = module_types[module_type].type;
	module->subtype = module_types[module_type].subtype;
	module->address = MOD_DRIVER_ADDRESS_NONE;
	module->stack_size = 0;

	if( module->type == MOD_THREAD_ID ) { module->status |= (1 << module->subtype); }
}

static bool moduleTypeParse(const initrc_command_t *command, add_module_type_t *module_type)
{
	for( size_t i = 0; i < (sizeof(module_types) / sizeof(module_types[0])); i++ )
	{
		if( strcmp(command->tok->tokens[1], module_types[i].name) == 0 )
		{
			*module_type = (add_module_type_t)i;
			return true;
		}
	}

	AUTOCODE_MSG_ERROR("unknown module type [%s:%i] %s",
					   command->initrc_name,
					   command->file_line_number,
					   command->tok->tokens[1]);
	return false;
}

static bool moduleOptionsParse(const initrc_command_t *command,
							   module_item_t *module,
							   unsigned int *option_count)
{
	if( (command->tok->count < 5) || ((command->tok->count % 2) == 0) )
	{
		AUTOCODE_MSG_ERROR("wrong token count [%s:%i] is %i, should be odd and at least 5",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->count);
		return false;
	}

	const add_module_option_context_t context = {.module = module,
												 .source_path = command->source_path};
	bool module_is_valid = true;
	for( int token = 3; token < command->tok->count; token += 2 )
	{
		bool option_found = false;
		for( size_t option = 0; option < ADD_MODULE_OPTION_COUNT; option++ )
		{
			if( strcmp(command->tok->tokens[token], module_options[option].name) != 0 )
			{
				continue;
			}

			option_found = true;
			if( (*module_options[option].func)(command->tok->tokens[token + 1], &context) ==
				ADD_MODULE_OPTION_OK )
			{
				option_count[option]++;
			}
			else
			{
				AUTOCODE_MSG_ERROR("unknown data [%s:%i] %s for option %s",
								   command->initrc_name,
								   command->file_line_number,
								   command->tok->tokens[token + 1],
								   command->tok->tokens[token]);
				module_is_valid = false;
			}
			break;
		}

		if( option_found == false )
		{
			AUTOCODE_MSG_ERROR("unknown option [%s:%i] %s",
							   command->initrc_name,
							   command->file_line_number,
							   command->tok->tokens[token]);
			module_is_valid = false;
		}
	}
	return module_is_valid;
}

static bool moduleOptionsValidate(const char *name,
							  const add_module_type_t module_type,
							  const unsigned int *option_count)
{
	bool module_is_valid = true;
	for( size_t option = 0; option < ADD_MODULE_OPTION_COUNT; option++ )
	{
		const option_cardinality_t cardinality =
			module_options[option].cardinality[module_type];
		if( (cardinality == OPTION_REQUIRED) && (option_count[option] == 0) )
		{
			AUTOCODE_MSG_ERROR(
				"Module %s : %s option is not set", name, module_options[option].name);
			module_is_valid = false;
		}
		if( (cardinality == OPTION_FORBIDDEN) && (option_count[option] > 0) )
		{
			AUTOCODE_MSG_ERROR("Module %s : %s option is not valid for %s modules",
							   name,
							   module_options[option].name,
							   module_types[module_type].name);
			module_is_valid = false;
		}
		else if( (cardinality != OPTION_CUMULATIVE) && (option_count[option] > 1) )
		{
			AUTOCODE_MSG_ERROR(
				"Module %s : %s option is multiple set", name, module_options[option].name);
			module_is_valid = false;
		}
	}

	if( (option_count[ADD_MODULE_OPTION_SOURCE_FILE] +
		 option_count[ADD_MODULE_OPTION_SOURCE_DIR]) == 0 )
	{
		AUTOCODE_MSG_ERROR("Module %s : -source_file or -source_dir option is not set", name);
		module_is_valid = false;
	}
	return module_is_valid;
}

static void moduleAdd(const initrc_command_t *command, const module_item_t *module)
{
	const char *name = command->tok->tokens[2];
	if( autoCodeBufferStringFits(name, sizeof(module->name)) == false )
	{
		AUTOCODE_MSG_ERROR(
			"Name too long <%s> (maximum %zu characters)", name, sizeof(module->name) - 1U);
		return;
	}

	AUTOCODE_MSG_INFO("found module : %s", name);
	module_type_t *module_list = &command->data_base->modules_type[module->type];
	for( int i = 0; i < module_list->modules_count; i++ )
	{
		if( strcmp(module_list->modules[i].name, name) == 0 )
		{
			AUTOCODE_MSG_ERROR("duplicate name [%s:%i] %s\n\n",
							   command->initrc_name,
							   command->file_line_number,
							   name);
			return;
		}
	}

	const int index = module_list->modules_count;
	if( index > MOD_COUNT_MAX - 1 )
	{
		AUTOCODE_MSG_ERROR("too much modules > %i type=%i\n", index, module->type);
		return;
	}

	snprintf(module_list->modules[index].name,
			 sizeof(module_list->modules[index].name),
			 "%s",
			 name);
	module_list->modules[index].status = module->status;
	module_list->modules[index].subtype = module->subtype;
	module_list->modules[index].address = module->address;
	module_list->modules[index].stack_size = module->stack_size;
	module_list->modules_count = index + 1;
}

void initrcAddModule(const initrc_command_t *command)
{
	if( command->tok->count < 3 )
	{
		AUTOCODE_MSG_ERROR("wrong token count [%s:%i] is %i, should be odd and at least 5",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->count);
		return;
	}

	add_module_type_t module_type;
	if( moduleTypeParse(command, &module_type) == false ) { return; }

	module_item_t module;
	moduleInit(&module, module_type);
	unsigned int option_count[ADD_MODULE_OPTION_COUNT] = {0};
	if( moduleOptionsParse(command, &module, option_count) == false ) { return; }
	if( moduleOptionsValidate(command->tok->tokens[2], module_type, option_count) == false )
	{
		return;
	}
	moduleAdd(command, &module);
}
