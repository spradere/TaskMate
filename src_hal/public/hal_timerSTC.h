/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_timerSTC.h
 * @brief public hal_timerSTC header declarations.
 *
 */

#ifndef HAL_TIMERSTC_H
#define HAL_TIMERSTC_H

#if defined(MCU_atmega2560)
	#include "mcu/atmega2560/hal_timerSTC_impl.h"
	#define HAL_TIMERSTC
#endif

#if !defined(HAL_TIMERSTC)
	#error "No hal implementation for Software Time Counter on selected hardware target."
#endif
#endif
