/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sys_hal_init.h
 * @brief hal init header declarations.
 */

#ifndef SYSCORE_HAL_INIT_H
#define SYSCORE_HAL_INIT_H

#if !defined(HAL_SYSTEM_CRITICAL_ALLOWED)
	#error "NOT ALLOWED INCLUDE : sys_hal_init.h"
#endif

/* ============================================================================
 * Generated includes
 * ========================================================================== */

// [autoCode_tag] hal_init
#include "hal_init.inc"
// [/tag]

#endif // SYSCORE_HAL_INIT_H
