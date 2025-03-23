/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file TaskMate_private_extern.h
 * @brief extrenal header to cut main sys core in multiple files.
 *
 * @todo Nothing
 */

#ifndef TASKMATE_PRIVATE_EXTERN_H
#define TASKMATE_PRIVATE_EXTERN_H

#include "sysCore/TaskMate_define.h"
#include "sysCore/error.h"

extern uint8_t DRIVER_COUNT;
extern driver_table_t driver_table[];

extern uint8_t TASK_COUNT;
extern task_table_t task_table[];
extern uint8_t task_current;

#endif
