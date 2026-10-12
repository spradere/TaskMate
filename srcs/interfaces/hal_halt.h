/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_halt.h
 * @brief Hardware-independent halt contract.
 */

#ifndef INTERFACES_HAL_HALT_H
#define INTERFACES_HAL_HALT_H

#include "interfaces/tm_cpp_compat.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Stop execution without returning.
 */
TM_NORETURN void hal_halt(void);

#ifdef __cplusplus
}
#endif

#endif // INTERFACES_HAL_HALT_H
