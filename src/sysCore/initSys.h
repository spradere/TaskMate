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
 * @file initSys.h
 * @brief Header for init system 
 *  
 * @todo Nothing
 */
 
 #ifndef INIT_SYS_H
 #define INIT_SYS_H
 
void taskCreate(void (*)(void), uint8_t);

void initTasks(void);
void initDrivers(void);

 #endif
