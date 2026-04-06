/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file gpio_signals.h
 * @brief interfaces gpio_signals header declarations.
 *
 */

#ifndef GPIO_SIGNALS_H
#define GPIO_SIGNALS_H

typedef enum
{
	GPIO_SIGNAL_INBOARD_LED,
	GPIO_SIGNAL_TASK1_LED,
	GPIO_SIGNAL_TASK2_LED,
	GPIO_SIGNAL_COUNT
} gpio_signal_t;

#endif
