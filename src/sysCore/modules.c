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
 * @file modules.c
 * @brief module data base
 *
 */

#include "sysCore/autoAlloc.h" // get DRIVERS_COUNT & THREADS_COUNT
#include "sysCore/modules.h"

static struct
{
	module_item_driver_t drivers[DRIVERS_COUNT];
	module_item_thread_t threads[THREADS_COUNT];
	uint8_t thread_current;
} modules;

void moduleThreadSetCurrent(uint8_t n) { modules.thread_current = n; }
uint8_t moduleThreadGetCurrent(void) { return modules.thread_current; }

void moduleThreadSetRTC(uint16_t count) { modules.threads[modules.thread_current].real_time_counter = count; }
uint16_t moduleThreadGetRTC(void) { return modules.threads[modules.thread_current].real_time_counter; }

module_item_driver_t *moduleDriverGetPointer(uint8_t id) { return &modules.drivers[id]; }
module_item_thread_t *moduleThreadGetPointer(uint8_t id) { return &modules.threads[id]; }
