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
 * @file sysCall.h
 * @brief sysCall header
 *
 * - This file contains sys call for threads.
 * - This layer prevent thread to mess up system.
 *
 * @todo Nothing
 */

#ifndef SYSCALL_H
#define SYSCALL_H

/**
 * @brief Get thread ID
 *
 * @param none, calling thread is deternined by sysCall, not by the thread
 * itself.
 * @return thread ID
 */
uint8_t sysCallGetThreadID(void);

/** @defgroup RTC Real Time Clock
 *
 * Each thread have one 16 bits time counter sycronized wtih other tasks.
 * If not zero the counter is decremented at 10 ms rate.
 */
/** @{ */

/**
 * @brief set TC sys call
 *
 * @param RTC counter value.
 * @return nothing.
 */
void sysCallSetThreadTC(uint16_t count);

/**
 * @brief get RTC sys call
 *
 * @param none, calling thread is deternined by sysCall, not the thread itself.
 * @return TC counter load value.
 */

uint16_t sysCallGetThreadTC(void);
/** @} */

/**
 * @brief yield hand to scheduler
 *
 * Used for cooperative mode, yield hand to scheduler by overflowing timer couter,
 * so the next thread will have a full time slice without adding more code.
 *
 * @param none.
 * @return nothing.
 */
void sysCallyieldHand(void);

#endif
