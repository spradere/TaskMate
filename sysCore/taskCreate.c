/**
 * @file taskCreate.c
 * @brief Implements taskcreate.
 * 
 * This file contains task stack initialisation 
 */

#include <avr/io.h>

#include "sysCore/TaskMate_private_extern.h"
#include "sysCore/taskCreate.h"


void taskCreate(void (*taskFunction)(void), uint8_t task_id)
{
	task_table[task_id].task_id=task_id;
	
	// RTC init
	task_table[task_id].task_RTC=0;
	
	// stack init
	task_table[task_id].stack_pointer = &task_table[task_id].stack[TASK_STACK_SIZE - 1]; // get to of stack
	*(task_table[task_id].stack_pointer--) = (uint16_t)taskFunction & 0xFF; //PCL; 
	*(task_table[task_id].stack_pointer--) = ((uint16_t)taskFunction >> 8) & 0xFF; //PCH
	*(task_table[task_id].stack_pointer--) = 0x00; //PCHH always 0 if code size < 128k 
	*(task_table[task_id].stack_pointer--) = 0x00; //R0                        
	*(task_table[task_id].stack_pointer--) = SREG;                        
	
	// Registers R1-R31
	for (int i = 1; i < 32; i++) 
	{*(task_table[task_id].stack_pointer--) = 0x00;}
	
	return;
}	
