/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file freebsd_halt.cpp
 * @brief FreeBSD halt implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "interfaces/hal_halt.h"

#include <cstdlib>

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

TM_NORETURN void hal_halt(void) { std::exit(EXIT_FAILURE); }
