/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD 2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sysCall.c
 * @brief sysCall implementation.
 *
 * @warning This file contains ATOMIC material !
 *
 */

#include "sysCall/sysCall.h"

#include <util/atomic.h>

#include "sysCore/modules.h"
#include "sysCore/tm_scheduler.h"

static uint8_t system_status = 0;

void sc_threadSetSTC(uint16_t count)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) { mod_threadSetSTC(count); }
}

uint16_t sc_threadGetSTC(void)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) { return mod_threadGetSTC(); }
	return 0; // dummy return to avoid -Wno-return-type
}

void sc_handYield(void)
{
	// tm_schedulerCoop();
}

void sc_flagClear(uint8_t flag) { system_status &= ~flag; }
void sc_flagSet(uint8_t flag) { system_status |= flag; }
uint8_t sc_flagGet(uint8_t flag) { return system_status &= flag; }
