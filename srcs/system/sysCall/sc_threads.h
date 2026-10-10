/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sc_threads.h
 * @brief Thread and system syscall declarations.
 */

#ifndef SYSCALL_SC_THREADS_H
#define SYSCALL_SC_THREADS_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <stdint.h>

#include "interfaces/tm_mod.h"
#include "interfaces/tm_string.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/* -----------------------------------------------
 * Run level
 * ---------------------------------------------*/

/**
 * @brief Get the current system run level.
 */
uint8_t sc_runLevelGet(void);
/**
 * @brief Set the system run level.
 * @return true if the requested run level was accepted.
 */
bool sc_runLevelSet(uint8_t run_level);

/* -----------------------------------------------
 * Software time counter
 * ---------------------------------------------*/

/**
 * @brief Set the current thread software time counter.
 */
void sc_threadSetSTC(uint16_t count);
/**
 * @brief Get the current thread software time counter.
 */
uint16_t sc_threadGetSTC(void);

/* -----------------------------------------------
 * Thread life cycle
 * ---------------------------------------------*/

/**
 * @brief Get the number of configured threads.
 */
mod_count_t sc_threadGetCount(void);
/**
 * @brief Read metadata for a thread identifier.
 * @param id Thread identifier below sc_threadGetCount().
 * @param[out] name Receives the thread name.
 * @param[out] run_level Receives the current run level.
 * @param[out] stack_size_bytes Receives configured stack size in bytes.
 * @return true if the identifier and output pointers are valid.
 */
bool sc_threadGetInfo(mod_count_t id, const tm_string_t **name, uint8_t *run_level,
					  uint16_t *stack_size_bytes);
/**
 * @brief Measure a thread's used stack space.
 * @param id Thread identifier below sc_threadGetCount().
 * @param[out] depth_bytes Receives used stack depth in bytes.
 * @return true if the identifier and output pointer are valid.
 */
bool sc_threadGetStackDepth(mod_count_t id, uint16_t *depth_bytes);
/**
 * @brief Mark the current thread as initialized.
 */
void sc_threadSetInitialized(void);
/**
 * @brief Start a thread by name at an initial run level.
 * @return true if the named thread was started.
 */
bool sc_threadStart(const char *name, uint8_t initial_run_level);
/**
 * @brief Stop a thread by name.
 * @return true if the named thread was stopped.
 */
bool sc_threadStop(const char *name);
/**
 * @brief Check whether all threads in a run level are ready.
 * @return true if all threads in the run level are ready.
 */
bool sc_threadRunLevelIsReady(uint8_t run_level);

/* -----------------------------------------------
 * Cooperative scheduling
 * ---------------------------------------------*/

/**
 * @brief Yield the current thread to the scheduler.
 */
void sc_coopYield(void);

#endif // SYSCALL_SC_THREADS_H
