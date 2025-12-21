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
 * @file hal_system_api.h
 * @brief hal system header api
 *
 */

#ifndef HAL_SYSTEM_CRITICAL_API_H
#define HAL_SYSTEM_CRITICAL_API_H

#include <stdint.h>

#ifndef HAL_SYSTEM_CRITICAL_API_ALLOWED
#error "include hal system critical api not allowed"
#endif

// hal autoInclude
#include "hal/autoInclude_hal_target.h"
#include "hal/autoInclude_hal_system_critical.h"

// arch
#include "hal/arch/avr8/hal_threadContextInit.h"

// mcu
#include "hal/mcu/atmega2560/hal_timerScheduler.h"
#include "hal/mcu/atmega2560/hal_timerRTC.h"

#endif
