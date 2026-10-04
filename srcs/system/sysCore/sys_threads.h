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

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Initialize the static thread database.
 */
void mod_threadsAlloc(void);

/**
 * @brief Set the current thread identifier.
 */
void mod_threadSetCurrent(uint8_t id);
/**
 * @brief Get the current thread identifier.
 */
uint8_t mod_threadGetCurrent(void);

/**
 * @brief Set the current thread software time counter.
 */
void mod_threadSetSTC(uint16_t count);
/**
 * @brief Get the current thread software time counter.
 */
uint16_t mod_threadGetSTC(void);
/**
 * @brief Advance software time counters for active threads.
 */
void mod_threadsTickSTC(void);

/**
 * @brief Get a thread's run level.
 */
uint8_t mod_threadRunLevelGet(uint8_t id);
/**
 * @brief Get a thread's configured stack size.
 */
uint16_t mod_threadStackSizeGet(uint8_t id);
/**
 * @brief Measure a thread's used stack space.
 * @param id Thread identifier.
 * @return Used stack depth in bytes.
 */
uint16_t mod_threadStackDepthGet(uint8_t id);

/**
 * @brief Mark a thread as initialized.
 */
void mod_threadSetInitialized(uint8_t id);
/**
 * @brief Start a thread at an initial run level.
 */
void mod_threadStart(uint8_t id, uint8_t initial_run_level);
/**
 * @brief Stop a thread.
 */
void mod_threadStop(uint8_t id);
/**
 * @brief Check whether a run level's threads are ready.
 * @return true if all threads in the run level are ready.
 */
bool mod_threadsRunLevelIsReady(uint8_t run_level);

/**
 * @brief Mark a thread as cooperatively yielded.
 */
void mod_threadSetYielded(uint8_t id);
/**
 * @brief Check whether a thread has yielded.
 * @return true if the thread has yielded.
 */
bool mod_threadIsYielded(uint8_t id);

#endif // SYSCORE_SYS_THREADS_H
