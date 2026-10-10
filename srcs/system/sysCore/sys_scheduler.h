/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sys_scheduler.h
 * @brief tm scheduler header declarations.
 *
 */

#ifndef SYSCORE_SYS_SCHEDULER_H
#define SYSCORE_SYS_SCHEDULER_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <stdint.h>

#include "interfaces/tm_runLevel.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Initialize scheduler state.
 */
void tm_schedulerInit(void);
/**
 * @brief Start scheduling configured threads.
 */
void tm_schedulerStart(void);
/**
 * @brief Yield through the cooperative scheduler path.
 */
void tm_schedulerCoop(void);
/**
 * @brief Set the scheduler run level.
 * @return true if the run level was accepted.
 */
bool tm_schedulerRunLevelSet(tm_run_level_t run_level);
/**
 * @brief Get the scheduler run level.
 */
tm_run_level_t tm_schedulerRunLevelGet(void);

#endif // SYSCORE_SYS_SCHEDULER_H
