/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
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
