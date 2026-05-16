/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file usart.h
 * @brief public hal_usart header declarations.
 *
 */

#ifndef HAL_PUBLIC_USART_H
#define HAL_PUBLIC_USART_H

#if defined(MCU_atmega2560)
	#include "hal/mcu/atmega2560/usart.h"
	#define HAL_USART
#endif

#if !defined(HAL_USART)
	#error "No hal implementation for usart on selected hardware target."
#endif

#endif // HAL_PUBLIC_USART_H
