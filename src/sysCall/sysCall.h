/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file sysCall.h
 * @brief sysCall header declarations.
 *
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

void sc_threadSetSTC(uint16_t count);
uint16_t sc_threadGetSTC(void);

void sc_handYield(void);

// system status flags
#define FLAG_DUMMY 0x01

void sc_flagClear(uint8_t flag);
void sc_flagSet(uint8_t flag);
uint8_t sc_flagGet(uint8_t flag);

#endif
