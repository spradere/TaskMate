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

/**
 * @brief Maximum number of modules in the static database.
 */
#define MOD_COUNT_MAX 256

/**
 * @name Module type identifiers
 * @brief Select driver, thread, system thread, or user thread records.
 * @{
 */
#define MOD_DRIVER_ID 0
#define MOD_THREAD_ID 1
#define MOD_THREAD_SYS_ID 2
#define MOD_THREAD_USER_ID 3
/** @} */

/**
 * @name Module database limits
 * @brief Define module counts, name sizes, and I2C address metadata.
 * @{
 */
#define MOD_TYPE_COUNT 2
#define MOD_NAME_SIZE_MAX 32
#define MOD_I2C_ADDRESS_MAX 0x7Eu
#define MOD_DRIVER_ADDRESS_NONE 0xFFu
/** @} */

#endif // INTERFACES_TM_MODULES_H
