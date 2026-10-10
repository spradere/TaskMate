/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_mod.h
 * @brief Module database contract declarations.
 */

#ifndef INTERFACES_TM_MOD_H
#define INTERFACES_TM_MOD_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/* -----------------------------------------------
 * Generated module count type and configured counts
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
 * @brief Selects the driver or thread database.
 */
typedef enum __attribute__((packed))
{
	MOD_DRIVER_ID = 0U,
	MOD_THREAD_ID,
	MOD_TYPE_COUNT
} mod_type_id_t;
_Static_assert(sizeof(mod_type_id_t) == 1U, "mod_type_id_t must be one byte");

/**
 * @name Module database limits
 * @brief Define module name sizes and I2C address metadata.
 * @{
 */
#define MOD_NAME_SIZE_MAX 32
#define MOD_I2C_ADDRESS_MAX 0x7Eu
#define MOD_DRIVER_ADDRESS_NONE 0xFFu
/** @} */

#endif // INTERFACES_TM_MOD_H
