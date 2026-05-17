/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file timerSTC.h
 * @brief timer stc header declarations.
 *
 */

#ifndef HAL_PUBLIC_TIMERSTC_H
#define HAL_PUBLIC_TIMERSTC_H

#if defined(MCU_atmega2560)
	#include "hal/mcu/atmega2560/timerSTC.h"
	#define HAL_TIMERSTC
#endif

#if !defined(HAL_TIMERSTC)
	#error "No hal implementation for Software Time Counter on selected hardware target."
#endif

#endif // HAL_PUBLIC_TIMERSTC_H
