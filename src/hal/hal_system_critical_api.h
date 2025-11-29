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

#include <stdint.h>

#ifndef HAL_SYSTEM_CRITICAL_ALLOWED
	#error "hal system critical not allowed"
#endif

#include "hal/autoInclude_hal_target.h"
#include "hal/autoInclude_hal_system_critical.h"

#include "sysCall/error.h"

void hal_archInit();
void hal_mcuInit();
void hal_boardInit();

// arch
void hal_threadContextInit(void (*func)(void), stack_word_t **stack_pointer, stack_word_t *stack_top);

// mcu
void hal_timerSchedulerInit();
void hal_timerSchedulerStart();
void hal_timerSchedulerStop();
void hal_timerSchedulerLoad();

void hal_timerRTCInit();
void hal_timerRTCStart();
void hal_timerRTCStop();
