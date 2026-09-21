/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file parseInitrc.c
 * @brief parse initrc implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "parseInitrc.h"

#include <errno.h>

#include "fileUtility.h"
#include "initrcCmdDispatch.h"
#include "tokenizer.h"

/* =============================================================================
 * Declarations - Local types
 * ===========================================================================*/

typedef enum
{
	AC_INITRC_VERSION_EXPECT_MAJOR,
	AC_INITRC_VERSION_EXPECT_MINOR,
	AC_INITRC_VERSION_VALID,
	AC_INITRC_VERSION_INVALID
} initrc_version_state_t;

typedef struct
{
	modules_database_t *data_base;
	const tokenizer_t *tok;
	const char *initrc_name;
	const char *source_path;
	int file_line_number;
} initrc_command_t;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void funcAddDriver(const initrc_command_t *command);
static void funcAddService(const initrc_command_t *command);
static void funcAddTask(const initrc_command_t *command);

/* -----------------------------------------------
 * init.rc command dispatch table
 * ---------------------------------------------*/

#define INITRC_COMMAND(X)           \
	X("addDriver", funcAddDriver)     \
	X("addService", funcAddService)   \
	X("addTask", funcAddTask)

static const struct
{
	const char *name;
	void (*func)(const initrc_command_t *command);
} initrc_commands[] = {
#define X(s, f) {(s), (f)},
	INITRC_COMMAND(X)
#undef X
		{NULL, NULL}};

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static bool initrcVersionValueMatches(const char *value, const unsigned long expected)
{
	char *end;

	errno = 0;
	const unsigned long parsed = strtoul(value, &end, 10);
	if( (value[0] == 0) || (*end != 0) || (errno == ERANGE) ) { return false; }
	return parsed == expected;
}

static bool initrcVersionHeaderParse(const tokenizer_t *tok, initrc_version_state_t *state,
									 const char *initrc_name, const int file_line_number)
{
	if( *state == AC_INITRC_VERSION_EXPECT_MAJOR )
	{
		if( (file_line_number != 1) || (tok->count != 2) ||
			(strcmp(tok->tokens[0], "!set_initrc_ver_major") != 0) )
		{
			AUTOCODE_MSG_ERROR("first init.rc line [%s:%i] must be !set_initrc_ver_major %i",
							   initrc_name,
							   file_line_number,
							   AC_INITRC_EXPECTED_VER_MAJOR);
			*state = AC_INITRC_VERSION_INVALID;
			return false;
		}
		if( initrcVersionValueMatches(tok->tokens[1], AC_INITRC_EXPECTED_VER_MAJOR) == false )
		{
			AUTOCODE_MSG_ERROR("unsupported init.rc major syntax version [%s:%i] %s, expected %i",
							   initrc_name,
							   file_line_number,
							   tok->tokens[1],
							   AC_INITRC_EXPECTED_VER_MAJOR);
			*state = AC_INITRC_VERSION_INVALID;
			return false;
		}

		*state = AC_INITRC_VERSION_EXPECT_MINOR;
		return false;
	}

	if( *state == AC_INITRC_VERSION_EXPECT_MINOR )
	{
		if( (file_line_number != 2) || (tok->count != 2) ||
			(strcmp(tok->tokens[0], "!set_initrc_ver_minor") != 0) )
		{
			AUTOCODE_MSG_ERROR("second init.rc line [%s:%i] must be !set_initrc_ver_minor %i",
							   initrc_name,
							   file_line_number,
							   AC_INITRC_EXPECTED_VER_MINOR);
			*state = AC_INITRC_VERSION_INVALID;
			return false;
		}
		if( initrcVersionValueMatches(tok->tokens[1], AC_INITRC_EXPECTED_VER_MINOR) == false )
		{
			AUTOCODE_MSG_ERROR("unsupported init.rc minor syntax version [%s:%i] %s, expected %i",
							   initrc_name,
							   file_line_number,
							   tok->tokens[1],
							   AC_INITRC_EXPECTED_VER_MINOR);
			*state = AC_INITRC_VERSION_INVALID;
			return false;
		}

		*state = AC_INITRC_VERSION_VALID;
		return false;
	}

	if( *state == AC_INITRC_VERSION_INVALID ) { return false; }
	if( (tok->count > 0) && (tok->tokens[0][0] == '!') )
	{
		AUTOCODE_MSG_ERROR("init.rc version command outside header [%s:%i] %s",
						   initrc_name,
						   file_line_number,
						   tok->tokens[0]);
		return false;
	}

	return true;
}

static void moduleInit(module_item_t *mod, const unsigned char type, const unsigned char subtype)
{
	mod->status = 0;
	mod->type = type;
	mod->subtype = subtype;
	mod->address = MOD_DRIVER_ADDRESS_NONE;
	mod->stack_size = 0;
	mod->cnt_set_runlevel = 0;
	mod->cnt_set_address = 0;
	mod->cnt_set_stack_size = 0;
	mod->cnt_set_source = 0;

	if( type == MOD_THREAD_ID ) { mod->status |= (1 << subtype); }
}

static bool moduleOptionsParse(const initrc_command_t *command, module_item_t *mod)
{
	if( (command->tok->count < 4) || ((command->tok->count % 2) != 0) )
	{
		AUTOCODE_MSG_ERROR("wrong token count [%s:%i] is %i, should be even and at least 4",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->count);
		return false;
	}

	bool module_is_valid = true;
	for( int i = 2; i < command->tok->count; i += 2 )
	{
		initrc_dispatch_result_t result = initrcCmdDispatch(command->tok->tokens[i],
													 command->tok->tokens[i + 1],
													 command->source_path,
													 mod);
		if( result == INITRC_DISPATCH_UNKNOWN_COMMAND )
		{
			AUTOCODE_MSG_ERROR("unknown option [%s:%i] %s",
							   command->initrc_name,
							   command->file_line_number,
							   command->tok->tokens[i]);
			module_is_valid = false;
		}
		if( result == INITRC_DISPATCH_UNKNOWN_DATA )
		{
			AUTOCODE_MSG_ERROR("unknown data [%s:%i] %s for option %s",
							   command->initrc_name,
							   command->file_line_number,
							   command->tok->tokens[i + 1],
							   command->tok->tokens[i]);
			module_is_valid = false;
		}
	}

	return module_is_valid;
}

static bool moduleCommonOptionsValidate(const char *name, const module_item_t *mod)
{
	bool module_is_valid = true;

	if( mod->cnt_set_runlevel < 1 )
	{
		AUTOCODE_MSG_ERROR("Module %s : -run option is not set", name);
		module_is_valid = false;
	}
	if( mod->cnt_set_runlevel > 1 )
	{
		AUTOCODE_MSG_ERROR("Module %s : -run option is multiple set", name);
		module_is_valid = false;
	}
	if( mod->cnt_set_source < 1 )
	{
		AUTOCODE_MSG_ERROR("Module %s : -source_file or -source_dir option is not set", name);
		module_is_valid = false;
	}

	return module_is_valid;
}

static bool moduleDriverOptionsValidate(const char *name, const module_item_t *mod)
{
	bool module_is_valid = moduleCommonOptionsValidate(name, mod);

	if( mod->cnt_set_address > 1 )
	{
		AUTOCODE_MSG_ERROR("Module %s : -i2c option is multiple set", name);
		module_is_valid = false;
	}
	if( mod->cnt_set_stack_size > 0 )
	{
		AUTOCODE_MSG_ERROR("Module %s : -stack option is only valid for threads", name);
		module_is_valid = false;
	}

	return module_is_valid;
}

static bool moduleThreadOptionsValidate(const char *name, const module_item_t *mod)
{
	bool module_is_valid = moduleCommonOptionsValidate(name, mod);

	if( mod->cnt_set_stack_size < 1 )
	{
		AUTOCODE_MSG_ERROR("Module %s : -stack option is not set", name);
		module_is_valid = false;
	}
	if( mod->cnt_set_stack_size > 1 )
	{
		AUTOCODE_MSG_ERROR("Module %s : -stack option is multiple set", name);
		module_is_valid = false;
	}
	if( mod->cnt_set_address > 0 )
	{
		AUTOCODE_MSG_ERROR("Module %s : -i2c option is only valid for drivers", name);
		module_is_valid = false;
	}

	return module_is_valid;
}

static void moduleAdd(const initrc_command_t *command, const module_item_t *mod_tmp)
{
	const char *name = command->tok->tokens[1];
	if( autoCodeBufferStringFits(name, sizeof(mod_tmp->name)) == false )
	{
		AUTOCODE_MSG_ERROR(
			"Name too long <%s> (maximum %zu characters)", name, sizeof(mod_tmp->name) - 1U);
		return;
	}

	AUTOCODE_MSG_INFO("found module : %s", name);

	module_type_t *mod = &command->data_base->modules_type[mod_tmp->type];
	for( int i = 0; i < mod->modules_count; i++ )
	{
		if( strcmp(mod->modules[i].name, name) == 0 )
		{
			AUTOCODE_MSG_ERROR("duplicate name [%s:%i] %s\n\n",
							   command->initrc_name,
							   command->file_line_number,
							   name);
			return;
		}
	}

	const int index = mod->modules_count;
	if( index > MOD_COUNT_MAX - 1 )
	{
		AUTOCODE_MSG_ERROR("too much modules > %i type=%i\n", index, mod_tmp->type);
		return;
	}

	snprintf(mod->modules[index].name, sizeof(mod->modules[index].name), "%s", name);
	mod->modules[index].status = mod_tmp->status;
	mod->modules[index].subtype = mod_tmp->subtype;
	mod->modules[index].address = mod_tmp->address;
	mod->modules[index].stack_size = mod_tmp->stack_size;
	mod->modules_count = index + 1;
}

static void funcAddDriver(const initrc_command_t *command)
{
	module_item_t mod_tmp;
	moduleInit(&mod_tmp, MOD_DRIVER_ID, 0);

	if( moduleOptionsParse(command, &mod_tmp) == false ) { return; }
	if( moduleDriverOptionsValidate(command->tok->tokens[1], &mod_tmp) == false ) { return; }
	moduleAdd(command, &mod_tmp);
}

static void funcAddService(const initrc_command_t *command)
{
	module_item_t mod_tmp;
	moduleInit(&mod_tmp, MOD_THREAD_ID, THREAD_BIT_TYPE_SYS);

	if( moduleOptionsParse(command, &mod_tmp) == false ) { return; }
	if( moduleThreadOptionsValidate(command->tok->tokens[1], &mod_tmp) == false ) { return; }
	moduleAdd(command, &mod_tmp);
}

static void funcAddTask(const initrc_command_t *command)
{
	module_item_t mod_tmp;
	moduleInit(&mod_tmp, MOD_THREAD_ID, THREAD_BIT_TYPE_USER);

	if( moduleOptionsParse(command, &mod_tmp) == false ) { return; }
	if( moduleThreadOptionsValidate(command->tok->tokens[1], &mod_tmp) == false ) { return; }
	moduleAdd(command, &mod_tmp);
}

static int initrcCommandDispatch(const char *name, const initrc_command_t *command)
{
	for( int i = 0; initrc_commands[i].name != NULL; i++ )
	{
		if( strcmp(name, initrc_commands[i].name) == 0 )
		{
			(*initrc_commands[i].func)(command);
			return 0;
		}
	}
	return -1;
}

void parseInitrc(modules_database_t *data_base, const char *initrc_name, const char *source_path)
{
	// Open list files
	AUTOCODE_MSG_INFO("open <%s>", initrc_name);

	file_t initrc_list;
	fileInit(&initrc_list);
	initrc_list.name = (char *)initrc_name;
	if( fileOpen(&initrc_list, "r", FILE_READONLY, __FILE__, __LINE__) != 0 ) { return; }

	// Variables
	int file_line_number = 0;
	tokenizer_t tok = {0};
	file_get_line_result_t line_result;
	initrc_version_state_t version_state = AC_INITRC_VERSION_EXPECT_MAJOR;

	while( (line_result = fileGetLine(&initrc_list, tok.line, sizeof(tok.line))) ==
		   FILE_GET_LINE_SUCCESS )
	{
		// Start
		file_line_number++;
		if( tokenizer(&tok) != 0 ) { continue; }
		const bool line_is_command =
			initrcVersionHeaderParse(&tok, &version_state, initrc_name, file_line_number);
		if( version_state == AC_INITRC_VERSION_INVALID ) { break; }
		if( line_is_command == false ) { continue; }

		// Process command
		if( (tok.count > 0) && (strcmp(tok.tokens[0], "#") != 0) ) // skip empty line or comment
		{
			const initrc_command_t command = {.data_base = data_base,
										  .tok = &tok,
										  .initrc_name = initrc_name,
										  .source_path = source_path,
										  .file_line_number = file_line_number};
			if( initrcCommandDispatch(tok.tokens[0], &command) != 0 )
			{
				AUTOCODE_MSG_ERROR(
					"unknown command [%s:%i] %s", initrc_name, file_line_number, tok.tokens[0]);
			}
		}
	}
	if( line_result == FILE_GET_LINE_ERROR )
	{
		AUTOCODE_MSG_ERROR("reading file <%s> after line %i", initrc_list.name, file_line_number);
	}
	if( version_state == AC_INITRC_VERSION_EXPECT_MAJOR )
	{
		AUTOCODE_MSG_ERROR("missing !set_initrc_ver_major %i on first line of init.rc file <%s>",
						   AC_INITRC_EXPECTED_VER_MAJOR,
						   initrc_name);
	}
	if( version_state == AC_INITRC_VERSION_EXPECT_MINOR )
	{
		AUTOCODE_MSG_ERROR("missing !set_initrc_ver_minor %i after init.rc major version <%s>",
						   AC_INITRC_EXPECTED_VER_MINOR,
						   initrc_name);
	}
	tokenizerFree(&tok);
	fileClose(&initrc_list, __FILE__, __LINE__);
}
