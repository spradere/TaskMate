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

void mod_threadsAlloc(void);

void mod_threadSetCurrent(uint8_t id);
uint8_t mod_threadGetCurrent(void);

void mod_threadSetSTC(uint16_t count);
uint16_t mod_threadGetSTC(void);
void mod_threadsTickSTC(void);

uint8_t mod_threadRunLevelGet(uint8_t id);
uint16_t mod_threadStackSizeGet(uint8_t id);
uint16_t mod_threadStackDepthGet(uint8_t id);

void mod_threadSetInitialized(uint8_t id);
void mod_threadStart(uint8_t id, uint8_t initial_run_level);
void mod_threadStop(uint8_t id);
bool mod_threadsRunLevelIsReady(uint8_t run_level);

void mod_threadSetYielded(uint8_t id);
bool mod_threadIsYielded(uint8_t id);

#endif // SYSCORE_SYS_THREADS_H
