/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sc_errors.c
 * @brief Error syscall implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "sc_errors.h"

#include <stddef.h>

#include "interfaces/hal_halt.h"
#include "system/sysCall/sc_string.h"

/* -----------------------------------------------
 * Generated error catalogue
 * ---------------------------------------------*/

// [autoCode_tag] error_catalog
#include "error_catalog.inc"
// [/tag]

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

const tm_string_t *err_getMessage(err_codes_t code)
{
	if( code >= ERROR_COUNT ) { return NULL; }
	return error_catalog[(error_count_t)code].name;
}

void sc_halt(void) { hal_halt(); }
