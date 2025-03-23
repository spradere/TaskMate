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
 * @file rtc.c
 * @brief Implements internal/external RTC.
 *
 * @todo Add external clock
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "TaskMate_private_extern.h"
#include "sysCore/rtc.h"

ISR(TIMER3_COMPA_vect)
{
	// RTC decrement
	for (uint8_t i = 0; i < TASK_COUNT; i++)
	{
		if (task_table[i].task_RTC > 0)
		{
			task_table[i].task_RTC--;
		}
	}
}
