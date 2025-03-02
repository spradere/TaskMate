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
 * @file taskCreate.h
 * @brief taskCreate header
 * 
 *   
 */
 

#ifndef TASKCREATE_H
#define TASKCREATE_H

/**
 * @brief Create task to be used by scheduler.
 * 
 * Initialize task start adress, stack and stack pointer.
 * 
 * @param Pointer to function, task ID
 * 
 * @return nothing.
 */
void taskCreate(void (*)(void), uint8_t);

#endif
