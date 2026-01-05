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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ! Auto generated code, do not edit !
// ! any changes will be lost         !
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef AUTO_HAL_SYSTEM_H
#define AUTO_HAL_SYSTEM_H

// target define
#include "hal/arch/avr8/arch_define.h"
#include "hal/mcu/atmega2560/mcu_define.h"
#include "hal/board/arduino_mega/board_define.h"

// target init
#include "hal/arch/avr8/hal_archInit.h"
#include "hal/mcu/atmega2560/hal_mcuInit.h"
#include "hal/board/arduino_mega/hal_boardInit.h"

// autoInclude hal headers tagged with // @hal_system
#include "hal/arch/avr8/hal_context.h"
#include "hal/arch/avr8/hal_stack.h"
#include "hal/mcu/atmega2560/hal_timerRTC.h"
#include "hal/mcu/atmega2560/hal_timerScheduler.h"

#endif
