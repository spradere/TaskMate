/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file autoCode.h
 * @brief auto code header declarations.
 *
 */

#ifndef AUTOCODE_AUTOCODE_H
#define AUTOCODE_AUTOCODE_H

/* ============================================================================
 * Constants
 * ========================================================================== */

#define AC_BUFFER_SIZE 256
#define AC_GENERATED_LINE_START 1000
#define AC_THREAD_STACK_SIZE_MIN 3UL
#define AC_SCLI_COMMAND_COUNT_MAX 16
#define AC_INITRC_EXPECTED_VER_MAJOR 1
#define AC_INITRC_EXPECTED_VER_MINOR 10
#define AC_AUTOCODE_VER_MAJOR 1
#define AC_AUTOCODE_VER_MINOR 6

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Get TaskMate definitions
#include "interfaces/tm_modules.h"
#include "interfaces/tm_runLevel.h"
#include "interfaces/tm_threads.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/* -----------------------------------------------
 * Message macros
 * ---------------------------------------------*/

#define AUTOCODE_MSG_ERROR(format, ...)                                                          \
	do {                                                                                         \
		fprintf(stderr, "[%s:%d] error : " format "\n", __FILE_NAME__, __LINE__, ##__VA_ARGS__); \
		perror("\t");                                                                            \
		autoCodeExit(AC_INCREMENT);                                                              \
	} while( 0 )

#define AUTOCODE_MSG_INFO(format, ...) \
	fprintf(stdout, "[%s] info : " format "\n", __FILE_NAME__, ##__VA_ARGS__)

/* -----------------------------------------------
 * Error handling
 * ---------------------------------------------*/

typedef enum
{
	AC_INCREMENT,
	AC_FORCE_EXIT
} ac_error_cmd_t;

/**
 * @brief Record an autoCode error or terminate generation.
 * @param cmd Increment the error count or force immediate exit.
 */
void autoCodeExit(ac_error_cmd_t cmd);

/* -----------------------------------------------
 * Module database types
 * ---------------------------------------------*/

/**
 * @brief Stores one parsed module's configuration.
 */
typedef struct
{
	char name[MOD_NAME_SIZE_MAX];
	unsigned char status;
	unsigned char type;
	unsigned char subtype;
	unsigned char address;
	uint16_t stack_size;

} module_item_t;

/**
 * @brief Groups modules of one type in the generated database.
 */
typedef struct
{
	module_item_t modules[MOD_COUNT_MAX];
	int modules_count;

} module_type_t;

/**
 * @brief Holds parsed modules and SCLI commands for code generation.
 */
typedef struct
{
	module_type_t modules_type[MOD_TYPE_COUNT];
	struct
	{
		struct
		{
			char name[MOD_NAME_SIZE_MAX];
			char function[MOD_NAME_SIZE_MAX];
		} commands[AC_SCLI_COMMAND_COUNT_MAX];
		uint8_t count;
	} scli;

} modules_database_t;

#endif // AUTOCODE_AUTOCODE_H
