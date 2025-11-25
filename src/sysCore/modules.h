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
 * @file modules.h
 * @brief module data base header
 *
 * @todo Nothing
 */

#ifndef MODULES_H
#define MODULES_H

#include "sysCore/modules_items.h"

void moduleThreadSetCurrent(uint8_t n);
uint8_t moduleThreadGetCurrent();

module_item_driver_t *moduleDriverGetPointer(uint8_t id);
module_item_thread_t *moduleThreadGetPointer(uint8_t id);

#endif
