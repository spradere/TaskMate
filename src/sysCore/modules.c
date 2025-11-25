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
 * @todo modules will be static + implement functions to acces module data base
 */

#include "sysCore/autoAlloc.h"
#include "sysCore/modules.h"

// todo static when all done
modules_t modules;

void moduleThreadSetCurrent(uint8_t n){ modules.thread_current = n;}
uint8_t moduleThreadGetCurrent(){ return modules.thread_current; }

module_item_driver_t *moduleDriverGetPointer(uint8_t id){ return &modules.drivers[id]; }
module_item_thread_t *moduleThreadGetPointer(uint8_t id){ return &modules.threads[id]; }
