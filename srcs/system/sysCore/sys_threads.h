/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sys_threads.h
 * @brief Static thread database declarations.
 */

#ifndef SYSCORE_SYS_THREADS_H
#define SYSCORE_SYS_THREADS_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <stdint.h>

#include "interfaces/tm_mod.h"
#include "interfaces/tm_runLevel.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Initialize the static thread database.
 */
void mod_threadsAlloc(void);

/**
 * @brief Set the current thread identifier.
 * @return true if the identifier is valid.
 */
bool mod_threadSetCurrent(mod_count_t id);
/**
 * @brief Get the current thread identifier.
 */
mod_count_t mod_threadGetCurrent(void);

/**
 * @brief Set the current thread software time counter.
 */
bool mod_threadSetSTC(uint16_t count);
/**
 * @brief Get the current thread software time counter.
 * @param[out] count Receives the counter value.
 */
bool mod_threadGetSTC(uint16_t *count);
/**
 * @brief Advance software time counters for active threads.
 */
void mod_threadTickSTC(void);
/**
 * @brief Set the delay before the current thread's next start, in milliseconds.
 * @return true if the current thread identifier is valid.
 */
bool mod_threadSetNextStartDelay_ms(uint16_t delay_ms);
/**
 * @brief Set the current thread's estimated WCET, in 100 us ticks.
 * @return true if the current thread identifier is valid.
 */
bool mod_threadSetWCET_100us(uint16_t ticks);
/**
 * @brief Set the current thread's deadline, in 100 us ticks.
 * @return true if the current thread identifier is valid.
 */
bool mod_threadSetDeadLine_100us(uint16_t ticks);

/**
 * @brief Get a thread's run level.
 * @param[out] run_level Receives the run level.
 */
bool mod_threadRunLevelGet(mod_count_t id, tm_run_level_t *run_level);
/**
 * @brief Get a thread's configured stack size.
 * @param[out] size_bytes Receives the stack size in bytes.
 */
bool mod_threadStackSizeGet(mod_count_t id, uint16_t *size_bytes);
/**
 * @brief Measure a thread's used stack space.
 * @param id Thread identifier.
 * @param[out] depth_bytes Receives used stack depth in bytes.
 */
bool mod_threadStackDepthGet(mod_count_t id, uint16_t *depth_bytes);

/**
 * @brief Mark a thread as initialized.
 */
bool mod_threadSetInitialized(mod_count_t id);
/**
 * @brief Start a thread at an initial run level.
 */
bool mod_threadStart(mod_count_t id, tm_run_level_t initial_run_level);
/**
 * @brief Stop a thread.
 */
bool mod_threadStop(mod_count_t id);
/**
 * @brief Check whether a run level's threads are ready.
 * @return true if all threads in the run level are ready.
 */
bool mod_threadsRunLevelIsReady(tm_run_level_t run_level);

/**
 * @brief Mark a thread as cooperatively yielded.
 */
bool mod_threadSetYielded(mod_count_t id);
/**
 * @brief Check whether a thread has yielded.
 * @param[out] yielded Receives the yielded state.
 */
bool mod_threadIsYielded(mod_count_t id, bool *yielded);

#endif // SYSCORE_SYS_THREADS_H
