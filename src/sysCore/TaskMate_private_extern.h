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
 * @file TaskMate_private_extern.h
 * @brief extrenal header to cut main sys core in multiple files.
 *
 * @todo Nothing
 */

#ifndef TASKMATE_PRIVATE_EXTERN_H
#define TASKMATE_PRIVATE_EXTERN_H

#include "sysCore/autoAlloc.h"
#include "sysCore/autoInclude_system.h"
#include "hal/autoInclude_hal.h"

#include "sysCall/sysCall.h"
#include "sysCall/error.h"

extern modules_t modules;

#endif
