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
 * @file sysCall.c
 * @brief Implements system calls.
 *
 * @warning This file contains ATOMIC material !
 *
 * @todo Add system call for driver lock, add runLevel() boot sequence, external
 * RTC cloc
 */

#include <avr/io.h>
#include <util/atomic.h>
#include "sysCore/TaskMate_private_extern.h"
#include "sysCore/sysCall.h"
#include "drivers/timer1.h"

uint8_t sysCallGetThreadID(void) { return (modules.threads[modules.thread_current].id); }

void sysCallSetThreadTC(uint16_t count)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) { modules.threads[modules.thread_current].time_counter = count; }
}

uint16_t sysCallGetThreadTC(void)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) { return (modules.threads[modules.thread_current].time_counter); }
}

void sysCallyieldHand(void)
{
	timer1LoadOverflow();
}
