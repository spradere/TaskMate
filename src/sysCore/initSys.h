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
 * @file initSys.h
 * @brief Header for init system
 *
 */

#ifndef INITSYS_H
#define INITSYS_H

void threadCreate(void (*func)(void), uint8_t num);

void initThreads(void);
void initDrivers(void);
void initRunLevels(void);

#endif
