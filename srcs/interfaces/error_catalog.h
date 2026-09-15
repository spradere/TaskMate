/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file error_catalog.h
 * @brief error catalog header declarations.
 */

#ifndef INTERFACES_ERROR_CATALOG_H
#define INTERFACES_ERROR_CATALOG_H

/* ============================================================================
 * Public definitions
 * ========================================================================== */

#ifdef AUTOCODE_BUILD
typedef enum
{
	ERROR_COUNT
} err_codes_t;
#else
// [autoCode_tag] error_enum
#include "error_enum.inc"
// [/tag]
#endif

#endif // INTERFACES_ERROR_CATALOG_H
