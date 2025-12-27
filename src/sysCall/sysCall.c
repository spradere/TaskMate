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
 */

#include "sysCall/sysCall.h"
#include <util/atomic.h>
#include "hal/autoInclude_hal_user.h"
#include "hal/autoInclude_hal_system_critical.h"
#include "sysCore/modules.h"

static uint8_t system_status = 0;

// [autoCode_tag] target name
const target_info_t target_info =
{
.arch = "avr8",
.mcu = "atmega2560",
.board = "arduino_mega"
};
// [/tag]

void sysCallGetTargetInfo(const target_info_t **target)
{
	*target = &target_info;
}

void sysCallSetThreadRTC(uint16_t count)
{
	// ATOMIC_BLOCK(ATOMIC_FORCEON) { modules.threads[modules.thread_current].real_time_counter = count; }
	ATOMIC_BLOCK(ATOMIC_FORCEON) { moduleThreadSetRTC(count); }
}

uint16_t sysCallGetThreadRTC(void)
{
	// ATOMIC_BLOCK(ATOMIC_FORCEON) { return (modules.threads[modules.thread_current].real_time_counter); }
	ATOMIC_BLOCK(ATOMIC_FORCEON) { return moduleThreadGetRTC(); }
}

void sysCallYieldHand(void) { hal_timerSchedulerLoad(); }

void sysCallClearFlag(uint8_t flag) { system_status &= ~flag; }
void sysCallSetFlag(uint8_t flag) { system_status |= flag; }
uint8_t sysCallGetFlag(uint8_t flag) { return system_status &= flag; }
