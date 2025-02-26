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
