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
 * @brief sysCall header
 *
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#include "tm_libc/tm_string.h"

typedef struct
{
	const tm_string_t arch;
	const tm_string_t mcu;
	const tm_string_t board;
} sc_target_info_t;

void sc_targetGetInfo(sc_target_info_t **target);

void sc_threadSetSTC(uint16_t count);
uint16_t sc_threadGetSTC(void);

void sc_handYield(void);

// system status flags
#define FLAG_COOP 0x01

void sc_flagClear(uint8_t flag);
void sc_flagSet(uint8_t flag);
uint8_t sc_flagGet(uint8_t flag);

#endif
