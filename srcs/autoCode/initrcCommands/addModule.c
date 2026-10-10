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

enum
{
	ADDMODULE_BASE_HEX = 16U,
	ADDMODULE_BASE_DECIMAL = 10U,
	ADDMODULE_COUNT_MIN = 5U,
	ADDMODULE_COUNT_HEADER = 3U,
	ADDMODULE_COUNT_PAIR = 2U,
	ADDMODULE_INDEX_TYPE = 1U,
	ADDMODULE_INDEX_NAME = 2U,
	ADDMODULE_COUNT_NEXT = 1U
};

/* =============================================================================
 * Declarations - Local types
 * ===========================================================================*/

typedef enum
{
	ADD_MODULE_OK,
	ADD_MODULE_INVALID_DATA
} add_module_result_t;

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
} add_module_context_t;

typedef add_module_result_t (*add_module_option_func_t)(const char *data,
														const add_module_context_t *context);

/* -----------------------------------------------
 * Module type dispatch table
 * ---------------------------------------------*/

#define ADD_MODULE_TYPE(X)                                    \
	X(DRIVER, "driver", MOD_DRIVER_ID, 0U)                     \
	X(SERVICE, "service", MOD_THREAD_ID, THREAD_BIT_TYPE_SYS) \
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

static add_module_result_t optionRun(const char *data, const add_module_context_t *context);
static add_module_result_t optionI2c(const char *data, const add_module_context_t *context);
static add_module_result_t optionStack(const char *data, const add_module_context_t *context);
static add_module_result_t optionSourceFile(const char *data, const add_module_context_t *context);
static add_module_result_t optionSourceDir(const char *data, const add_module_context_t *context);

#define ADD_MODULE_OPTION(X)                                                            \
	X(RUN, "-run", optionRun, OPTION_REQUIRED, OPTION_REQUIRED, OPTION_REQUIRED)        \
	X(I2C, "-i2c", optionI2c, OPTION_OPTIONAL, OPTION_FORBIDDEN, OPTION_FORBIDDEN)      \
	X(STACK, "-stack", optionStack, OPTION_FORBIDDEN, OPTION_REQUIRED, OPTION_REQUIRED) \
	X(SOURCE_FILE,                                                                      \
	  "-source_file",                                                                   \
	  optionSourceFile,                                                                 \
	  OPTION_CUMULATIVE,                                                                \
	  OPTION_CUMULATIVE,                                                                \
	  OPTION_CUMULATIVE)                                                                \
	X(SOURCE_DIR,                                                                       \
	  "-source_dir",                                                                    \
	  optionSourceDir,                                                                  \
	  OPTION_CUMULATIVE,                                                                \
	  OPTION_CUMULATIVE,                                                                \
	  OPTION_CUMULATIVE)

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

static add_module_result_t optionRun(const char *data, const add_module_context_t *context)
{
	if( strcmp(data, "none") == 0U ) { context->module->status |= RL_RUN_NONE; }
	else if( strcmp(data, "core") == 0U ) { context->module->status |= RL_RUN_CORE; }
	else if( strcmp(data, "driver") == 0U ) { context->module->status |= RL_RUN_DRIVER; }
	else if( strcmp(data, "service") == 0U ) { context->module->status |= RL_RUN_SERVICE; }
	else if( strcmp(data, "user") == 0U ) { context->module->status |= RL_RUN_USER; }
	else { return ADD_MODULE_INVALID_DATA; }

	return ADD_MODULE_OK;
}

static add_module_result_t optionI2c(const char *data, const add_module_context_t *context)
{
	char *end;
	const unsigned long address = strtoul(data, &end, ADDMODULE_BASE_HEX);

	if( (data[0U] == 0U) || (*end != 0U) || (address > MOD_I2C_ADDRESS_MAX) )
	{
		return ADD_MODULE_INVALID_DATA;
	}

	context->module->address = (unsigned char)address;
	return ADD_MODULE_OK;
}

static add_module_result_t optionStack(const char *data, const add_module_context_t *context)
{
	char *end;
	const unsigned long stack_size = strtoul(data, &end, ADDMODULE_BASE_DECIMAL);

	if( (data[0U] == 0U) || (*end != 0U) || (stack_size < AUTOCODE_SIZE_STACKMIN) ||
		(stack_size > UINT16_MAX) )
	{
		return ADD_MODULE_INVALID_DATA;
	}

	context->module->stack_size = (uint16_t)stack_size;
	return ADD_MODULE_OK;
}

static add_module_result_t optionSource(const char *data, const bool directory,
										const add_module_context_t *context)
{
	// Source declarations also drive the build list, so their paths must exist.
	char path[AUTOCODE_SIZE_BUFFER];
	const int length = snprintf(path, sizeof(path), "%s/%s", context->source_path, data);

	if( (length < (int)0U) || ((size_t)length >= sizeof(path)) ) { return ADD_MODULE_INVALID_DATA; }

	struct stat status;
	if( stat(path, &status) != 0U ) { return ADD_MODULE_INVALID_DATA; }
	if( directory && !S_ISDIR(status.st_mode) ) { return ADD_MODULE_INVALID_DATA; }
	if( !directory && !S_ISREG(status.st_mode) ) { return ADD_MODULE_INVALID_DATA; }
	return ADD_MODULE_OK;
}

static add_module_result_t optionSourceFile(const char *data, const add_module_context_t *context)
{
	return optionSource(data, false, context);
}

static add_module_result_t optionSourceDir(const char *data, const add_module_context_t *context)
{
	return optionSource(data, true, context);
}

static void moduleInit(module_item_t *module, const add_module_type_t module_type)
{
	module->status = 0U;
	module->type = module_types[module_type].type;
	module->subtype = module_types[module_type].subtype;
	module->address = MOD_DRIVER_ADDRESS_NONE;
	module->stack_size = 0U;

	if( module->type == MOD_THREAD_ID ) { module->status |= (1U << module->subtype); }
}

static bool moduleTypeParse(const initrc_command_t *command, add_module_type_t *module_type)
{
	for( size_t i = 0U; i < (sizeof(module_types) / sizeof(module_types[0U])); i++ )
	{
		if( strcmp(command->tok->tokens[ADDMODULE_INDEX_TYPE], module_types[i].name) == 0U )
		{
			*module_type = (add_module_type_t)i;
			return true;
		}
	}

	AUTOCODE_MSG_ERROR("unknown module type [%s:%i] %s",
					   command->initrc_name,
					   command->file_line_number,
					   command->tok->tokens[ADDMODULE_INDEX_TYPE]);
	return false;
}

static bool moduleOptionsParse(const initrc_command_t *command, module_item_t *module,
							   unsigned int *option_count)
{
	if( (command->tok->count < ADDMODULE_COUNT_MIN) ||
		((command->tok->count % ADDMODULE_COUNT_PAIR) == 0U) )
	{
		AUTOCODE_MSG_ERROR("wrong token count [%s:%i] is %i, should be odd and at least %i",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->count, ADDMODULE_COUNT_MIN);
		return false;
	}

	const add_module_context_t context = {.module = module, .source_path = command->source_path};
	bool module_is_valid = true;
	// Option names and values occur in pairs after the module type and name.
	for( int token = ADDMODULE_COUNT_HEADER; token < command->tok->count;
		 token += ADDMODULE_COUNT_PAIR )
	{
		bool option_found = false;
		for( size_t option = 0U; option < ADD_MODULE_OPTION_COUNT; option++ )
		{
			if( strcmp(command->tok->tokens[token], module_options[option].name) != 0U )
			{
				continue;
			}

			// Apply the matched value and count only options whose data is valid.
			option_found = true;
			if( (*module_options[option].func)(
					command->tok->tokens[token + ADDMODULE_COUNT_NEXT], &context) ==
				ADD_MODULE_OK )
			{
				option_count[option]++;
			}
			else
			{
				AUTOCODE_MSG_ERROR("unknown data [%s:%i] %s for option %s",
								   command->initrc_name,
								   command->file_line_number,
								   command->tok->tokens[token + ADDMODULE_COUNT_NEXT],
								   command->tok->tokens[token]);
				module_is_valid = false;
			}
			break;
		}

		// Continue through the remaining pairs to report all unknown options.
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

static bool moduleOptionsValidate(const char *name, const add_module_type_t module_type,
								  const unsigned int *option_count)
{
	bool module_is_valid = true;
	// Cardinality depends on whether the declaration is a driver, service, or task.
	for( size_t option = 0U; option < ADD_MODULE_OPTION_COUNT; option++ )
	{
		const option_cardinality_t cardinality = module_options[option].cardinality[module_type];
		if( (cardinality == OPTION_REQUIRED) && (option_count[option] == 0U) )
		{
			AUTOCODE_MSG_ERROR(
				"Module %s : %s option is not set", name, module_options[option].name);
			module_is_valid = false;
		}
		if( (cardinality == OPTION_FORBIDDEN) && (option_count[option] > 0U) )
		{
			AUTOCODE_MSG_ERROR("Module %s : %s option is not valid for %s modules",
							   name,
							   module_options[option].name,
							   module_types[module_type].name);
			module_is_valid = false;
		}
		else if( (cardinality != OPTION_CUMULATIVE) && (option_count[option] > 1U) )
		{
			AUTOCODE_MSG_ERROR(
				"Module %s : %s option is multiple set", name, module_options[option].name);
			module_is_valid = false;
		}
	}

	if( (option_count[ADD_MODULE_OPTION_SOURCE_FILE] +
		 option_count[ADD_MODULE_OPTION_SOURCE_DIR]) == 0U )
	{
		AUTOCODE_MSG_ERROR("Module %s : -source_file or -source_dir option is not set", name);
		module_is_valid = false;
	}
	return module_is_valid;
}

static void moduleAdd(const initrc_command_t *command, const module_item_t *module)
{
	// Commit the validated record only after checking the shared type list.
	const char *name = command->tok->tokens[ADDMODULE_INDEX_NAME];
	if( strlen(name) >= sizeof(module->name) - 1U )
	{
		AUTOCODE_MSG_ERROR(
			"Name too long <%s> (maximum %zu characters)", name, sizeof(module->name) - 1U);
		return;
	}

	AUTOCODE_MSG_INFO("found module : %s", name);
	module_type_t *module_list = &command->data_base->modules_type[module->type];
	// Names must be unique within the shared driver or thread list.
	for( size_t i = 0U; i < module_list->modules_count; i++ )
	{
		if( strcmp(module_list->modules[i].name, name) == 0U )
		{
			AUTOCODE_MSG_ERROR("duplicate name [%s:%i] %s\n\n",
							   command->initrc_name,
							   command->file_line_number,
							   name);
			return;
		}
	}

	if( command->data_base->module_count >= command->data_base->module_count_max )
	{
		AUTOCODE_MSG_ERROR("module count exceeds configured maximum %zu\n",
						   command->data_base->module_count_max);
		return;
	}

	const size_t index = module_list->modules_count;
	// Publish the record only after name and capacity checks have passed.
	snprintf(
		module_list->modules[index].name, sizeof(module_list->modules[index].name), "%s", name);
	module_list->modules[index].status = module->status;
	module_list->modules[index].subtype = module->subtype;
	module_list->modules[index].address = module->address;
	module_list->modules[index].stack_size = module->stack_size;
	module_list->modules_count = index + ADDMODULE_COUNT_NEXT;
	command->data_base->module_count++;
}

void initrcAddModule(const initrc_command_t *command)
{
	if( command->data_base->module_count_set == false )
	{
		AUTOCODE_MSG_ERROR("setModuleCount must be defined before addModule [%s:%i]",
						   command->initrc_name,
						   command->file_line_number);
		return;
	}

	if( command->tok->count < ADDMODULE_COUNT_HEADER )
	{
		AUTOCODE_MSG_ERROR("wrong token count [%s:%i] is %i, should be odd and at least %i",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->count, ADDMODULE_COUNT_MIN);
		return;
	}

	add_module_type_t module_type;
	if( moduleTypeParse(command, &module_type) == false ) { return; }

	module_item_t module;
	moduleInit(&module, module_type);
	unsigned int option_count[ADD_MODULE_OPTION_COUNT] = {0U};
	if( moduleOptionsParse(command, &module, option_count) == false ) { return; }
	if( moduleOptionsValidate(command->tok->tokens[ADDMODULE_INDEX_NAME],
							  module_type, option_count) == false )
	{
		return;
	}
	moduleAdd(command, &module);
}
