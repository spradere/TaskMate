/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file sysCall.h
 * @brief sysCall header
 *
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

typedef struct
{
	const char *arch;
	const char *mcu;
	const char *board;
} target_info_t;

void sysCallGetTargetInfo(const target_info_t **target);

void sysCallSetThreadRTC(uint16_t count);
uint16_t sysCallGetThreadRTC(void);

void sysCallYieldHand(void);

// system status flags
#define FLAG_COOP 0x01

void sysCallClearFlag(uint8_t flag);
void sysCallSetFlag(uint8_t flag);
uint8_t sysCallGetFlag(uint8_t flag);

#endif
