/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sysCall.h
 * @brief sys call header declarations.
 *
 */

#ifndef SYSCALL_SYSCALL_H
#define SYSCALL_SYSCALL_H

#include <stdbool.h>
#include <stdint.h>

void sc_threadSetSTC(uint16_t count);
uint16_t sc_threadGetSTC(void);

bool sc_threadStart(const char *name, uint8_t initial_run_level);
bool sc_threadStop(const char *name);

void sc_coopYield(void);

// system status flags
#define FLAG_DUMMY 0x01

void sc_flagClear(uint8_t flag);
void sc_flagSet(uint8_t flag);
uint8_t sc_flagGet(uint8_t flag);

#endif // SYSCALL_SYSCALL_H
