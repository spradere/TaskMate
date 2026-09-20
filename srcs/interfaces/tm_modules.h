/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_modules.h
 * @brief modules define header declarations.
 */

#ifndef INTERFACES_TM_MODULES_H
#define INTERFACES_TM_MODULES_H

/* ============================================================================
 * Includes
 * ========================================================================== */

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/* -----------------------------------------------
 * Generated module counts
 * ---------------------------------------------*/

#ifndef AUTOCODE_BUILD
	// [autoCode_tag] modules_count
#include "modules_count.inc"
// [/tag]
#endif

/* -----------------------------------------------
 * Module constants
 * ---------------------------------------------*/

#define MOD_COUNT_MAX 256

#define MOD_DRIVER_ID 0
#define MOD_THREAD_ID 1
#define MOD_THREAD_SYS_ID 2
#define MOD_THREAD_USER_ID 3

#define MOD_TYPE_COUNT 2
#define MOD_NAME_SIZE_MAX 32
#define MOD_I2C_ADDRESS_MAX 0x7Eu
#define MOD_DRIVER_ADDRESS_NONE 0xFFu

#define MOD_THREAD_STACK_SIZE 256

#endif // INTERFACES_TM_MODULES_H
