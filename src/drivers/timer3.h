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
 * @file timer3.h
 * @brief header of timer3 used for Real Time Clock
 *
 * @todo Nothing
 */

#ifndef TIMER3_H
#define TIMER3_H

#include <stdint.h>

void timer3SetStatus(uint8_t);
uint8_t timer3GetStatus(void);

void timer3Init(void);
void timer3Start(void);
void timer3Stop(void);

#endif
