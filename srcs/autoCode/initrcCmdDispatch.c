/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file initrcCmdDispatch.c
 * @brief initrc cmd dispatch implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "initrcCmdDispatch.h"

#include <sys/stat.h>

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Command handlers
 * ---------------------------------------------*/

static initrc_dispatch_result_t funcRun(const char *data, module_item_t *mod)
{
	if( strcmp(data, "none") == 0 ) { mod->status |= RL_RUN_NONE; }
	else if( strcmp(data, "core") == 0 ) { mod->status |= RL_RUN_CORE; }
	else if( strcmp(data, "driver") == 0 ) { mod->status |= RL_RUN_DRIVER; }
	else if( strcmp(data, "service") == 0 ) { mod->status |= RL_RUN_SERVICE; }
	else if( strcmp(data, "user") == 0 ) { mod->status |= RL_RUN_USER; }
	else { return INITRC_DISPATCH_UNKNOWN_DATA; }

	mod->cnt_set_runlevel++;
	return INITRC_DISPATCH_OK;
}

static initrc_dispatch_result_t funcI2cAddress(const char *data, module_item_t *mod)
{
	char *end;
	const unsigned long address = strtoul(data, &end, 16);

	if( (data[0] == 0) || (*end != 0) || (address > MOD_I2C_ADDRESS_MAX) )
	{
		return INITRC_DISPATCH_UNKNOWN_DATA;
	}

	mod->address = (unsigned char)address;
	mod->cnt_set_address++;
	return INITRC_DISPATCH_OK;
}

static initrc_dispatch_result_t funcStack(const char *data, module_item_t *mod)
{
	char *end;
	const unsigned long stack_size = strtoul(data, &end, 10);

	if( (data[0] == 0) || (*end != 0) || (stack_size < AC_THREAD_STACK_SIZE_MIN) ||
		(stack_size > UINT16_MAX) )
	{
		return INITRC_DISPATCH_UNKNOWN_DATA;
	}

	mod->stack_size = (uint16_t)stack_size;
	mod->cnt_set_stack_size++;
	return INITRC_DISPATCH_OK;
}

static initrc_dispatch_result_t funcSource(const char *data, const char *source_path,
										   const bool directory, module_item_t *mod)
{
	char path[AC_BUFFER_SIZE];
	const int length = snprintf(path, sizeof(path), "%s/%s", source_path, data);

	if( (length < 0) || ((size_t)length >= sizeof(path)) ) { return INITRC_DISPATCH_UNKNOWN_DATA; }

	struct stat status;
	if( stat(path, &status) != 0 ) { return INITRC_DISPATCH_UNKNOWN_DATA; }
	if( directory && !S_ISDIR(status.st_mode) ) { return INITRC_DISPATCH_UNKNOWN_DATA; }
	if( !directory && !S_ISREG(status.st_mode) ) { return INITRC_DISPATCH_UNKNOWN_DATA; }

	mod->cnt_set_source++;
	return INITRC_DISPATCH_OK;
}

static const initrc_cmd_t initrc_cmds[] = {
	{"-run", funcRun},
	{"-i2c", funcI2cAddress},
	{"-stack", funcStack},
	{NULL, NULL}};

/* -----------------------------------------------
 * Command dispatch
 * ---------------------------------------------*/

initrc_dispatch_result_t initrcCmdDispatch(const char *cmd, const char *data,
										   const char *source_path, module_item_t *mod)
{
	if( strcmp(cmd, "-source_file") == 0 ) { return funcSource(data, source_path, false, mod); }
	if( strcmp(cmd, "-source_dir") == 0 ) { return funcSource(data, source_path, true, mod); }

	for( int i = 0; initrc_cmds[i].name != NULL; i++ )
	{
		if( strcmp(cmd, initrc_cmds[i].name) == 0 ) { return (*initrc_cmds[i].func)(data, mod); }
	}
	return INITRC_DISPATCH_UNKNOWN_COMMAND;
}
