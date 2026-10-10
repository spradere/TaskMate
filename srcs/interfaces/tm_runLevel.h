/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_runLevel.h
 * @brief Run level contract declarations.
 */

#ifndef INTERFACES_TM_RUNLEVEL_H
#define INTERFACES_TM_RUNLEVEL_H

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Orders system startup from core through user tasks.
 */
typedef enum __attribute__((packed))
{
	RL_RUN_NONE = 0U,
	RL_RUN_CORE,
	RL_RUN_DRIVER,
	RL_RUN_SERVICE,
	RL_RUN_USER,
	RL_LEVEL_COUNT
} tm_run_level_t;
_Static_assert(sizeof(tm_run_level_t) == 1U, "tm_run_level_t must be one byte");

/**
 * @name Run level encoding
 * @brief Bound run levels and extract their status bit field.
 * @{
 */
#define RL_LEVEL_MASK 0x07
/** @} */
_Static_assert(RL_LEVEL_COUNT <= (RL_LEVEL_MASK + 1U), "run levels exceed status field");

/**
 * @brief Extract run level bits from a status value.
 */
#define RL_GET_RUN_LEVEL(status) ((tm_run_level_t)((status) & RL_LEVEL_MASK))

#endif // INTERFACES_TM_RUNLEVEL_H
