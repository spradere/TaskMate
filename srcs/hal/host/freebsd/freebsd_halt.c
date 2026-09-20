/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file freebsd_halt.c
 * @brief FreeBSD halt implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "interfaces/hal_halt.h"

#include <stdlib.h>

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

_Noreturn void hal_halt(void) { exit(EXIT_FAILURE); }
