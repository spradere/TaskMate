/**
 * @file sysCall.c
 * @brief Implements system calls.
 * 
 * @warning This file contains ATOMIC material !
 */
 
 #include <avr/io.h>
#include <util/atomic.h>
#include "sysCore/TaskMate_private_extern.h"
#include "sysCore/sysCall.h"


uint8_t sysCallGetTaskID(void)
{
	return(task_table[task_current].task_id);	
}


void sysCallSetTaskRTC(uint16_t count)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON){task_table[task_current].task_RTC=count;}
	return;
}

uint16_t sysCallGetTaskRTC(void)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON){return(task_table[task_current].task_RTC);}
}
