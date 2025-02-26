/**
 * @file sysCall.h
 * @brief sysCall header
 * 
 * - This file contains sys call for task.
 * - This layer prevent task to mess up system.
 * 
 */

#ifndef SYSCALL_H
#define SYSCALL_H

/**
 * @brief Get task ID
 * 
 * @param none, calling task is deternined by sysCall, not the task itself.
 * @return task ID
 */
uint8_t sysCallGetTaskID(void);



/** @defgroup RTC Real Time Clock
 * 
 * Each task have one 16 bits timer/counter sycronized wtih other tasks.
 * If not zero the counter is decremented at 10 ms rate.
 */
/** @{ */

/**
 * @brief set RTC sys call
 * 
 * @param RTC counter value.
 * @return nothing.
 */
void sysCallSetTaskRTC(uint16_t);

/**
 * @brief get RTC sys call
 * 
 * @param none, calling task is deternined by sysCall, not the task itself.
 * @return RTC counter load value.
 */
 
 uint16_t sysCallGetTaskRTC(void);
/** @} */
#endif
