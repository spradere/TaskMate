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
 * @file timer1.h
 * @brief header for timer1 used for preemptive sheduler
 *
 * @todo Nothing
 */

#ifndef TIMER1_H
#define TIMER1_H

#include <stdint.h>

void timer1SetStatus(uint8_t status);
uint8_t timer1GetStatus(void);

void timer1Init(void);
void timer1Start(void);
void timer1Stop(void);

#endif
