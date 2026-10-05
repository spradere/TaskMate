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

/**
 * @name autoCode buffer and table limits
 * @brief Set autoCode buffer, line, stack, and command constants.
 * @{
 */
#define AUTOCODE_SIZE_BUFFER 256U
#define AUTOCODE_LINE_GENERATED 1000U
#define AUTOCODE_SIZE_STACKMIN 3UL
#define AUTOCODE_COUNT_SCLIMAX 16U
/** @} */

/**
 * @name autoCode syntax versions
 * @brief Identify accepted init.rc syntax and the autoCode version.
 * @{
 */
#define AUTOCODE_VERSION_INITRCMAJOR 1U
#define AUTOCODE_VERSION_INITRCMINOR 10U
#define AUTOCODE_VERSION_MAJOR 1U
#define AUTOCODE_VERSION_MINOR 6U
/** @} */

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

/**
 * @name autoCode diagnostics
 * @brief Emit error or information messages during generation.
 * @{
 */
#define AUTOCODE_MSG_ERROR(format, ...)                                                          \
	do {                                                                                         \
		fprintf(stderr, "[%s:%d] error : " format "\n", __FILE_NAME__, __LINE__, ##__VA_ARGS__); \
		perror("\t");                                                                            \
		autoCodeExit(AC_INCREMENT);                                                              \
	} while( 0U )

#define AUTOCODE_MSG_INFO(format, ...) \
	fprintf(stdout, "[%s] info : " format "\n", __FILE_NAME__, ##__VA_ARGS__)
/** @} */

/* -----------------------------------------------
 * Error handling
 * ---------------------------------------------*/

/**
 * @brief Selects error counting or immediate termination.
 */
typedef enum
{
	AC_INCREMENT,
	AC_FORCE_EXIT
} ac_error_cmd_t;

/**
 * @brief Reports success or failure from an autoCode input operation.
 */
typedef enum
{
	AC_RESULT_OK = 0U,
	AC_RESULT_ERROR
} ac_result_t;

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
		} commands[AUTOCODE_COUNT_SCLIMAX];
		uint8_t count;
	} scli;

} modules_database_t;

#endif // AUTOCODE_AUTOCODE_H
