
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
 * @file TaskMate_error.h
 * @brief System wide error definitions
 * 
 * 
 */
 
#ifndef TASKMATE_ERROR_H
#define TASKMATE_ERROR_H

typedef enum {
    ERR_SUCCESS = 0,        // No error
    ERR_INVALID_ARG = 1,    // Non-critical: Task should handle it
    ERR_BUFFER_EMPTY = 2,   // Non-critical: Task should retry
    ERR_TIMEOUT = 3,        // Non-critical: Task can retry or abort
    
    ERR_FATAL_STACK_OVERFLOW = 100, // Critical: OS must take action
    ERR_FATAL_MEMORY_CORRUPTION = 101, // Critical: OS must kill task
    ERR_FATAL_HW_FAILURE = 102 // Critical: OS must halt or reboot
} ErrorCode;

void setCriticalError(ErrorCode);

#endif
