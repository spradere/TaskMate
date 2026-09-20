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

#include "interfaces/tm_string.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/* -----------------------------------------------
 * Run level
 * ---------------------------------------------*/

uint8_t sc_runLevelGet(void);
bool sc_runLevelSet(uint8_t run_level);

/* -----------------------------------------------
 * Software time counter
 * ---------------------------------------------*/

void sc_threadSetSTC(uint16_t count);
uint16_t sc_threadGetSTC(void);

/* -----------------------------------------------
 * Thread life cycle
 * ---------------------------------------------*/

uint16_t sc_threadGetCount(void);
bool sc_threadGetInfo(uint16_t id, const tm_string_t **name, uint8_t *run_level,
					  uint16_t *stack_size_bytes);
bool sc_threadGetStackDepth(uint16_t id, uint16_t *depth_bytes);
void sc_threadSetInitialized(void);
bool sc_threadStart(const char *name, uint8_t initial_run_level);
bool sc_threadStop(const char *name);
bool sc_threadRunLevelIsReady(uint8_t run_level);

/* -----------------------------------------------
 * Cooperative scheduling
 * ---------------------------------------------*/

void sc_coopYield(void);

#endif // SYSCALL_SC_THREADS_H
