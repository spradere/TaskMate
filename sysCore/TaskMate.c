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
 * @file TaskMate.c
 * @brief Implements TaskMate preemptive scheduler & RTC.
 * 
 * This file contains : 
 * - system, drivers and task initialisation
 * - the core scheduling algorithm, handling task switching and preemptive multitasking.
 * 
 * @warning do not edit code between tag it will be deleted by automatic generated code !
 * 
 * @note tag format are one line C comment // + [tag]
 * 
 * - task / driver
 * - include / allocate / init
 * 
 * @todo more documentation
 */
 
#include <avr/io.h>
#include <avr/interrupt.h>

#include "sysCore/TaskMate_define.h"
#include "sysCore/taskCreate.h"


// do not edit code between tag : automatic generated code !
// [tag] driver include
#include "drivers/timer1.h"
#include "drivers/timer3.h"
// [/tag]

// do not edit code between tag : automatic generated code !
// [tag] task include
#include "tasks/task1.h"
#include "tasks/task2.h"
// [/tag]

// do not edit code between tag : automatic generated code !
// [tag] driver alloc
#define DRIVER_COUNT 2
driver_table_t driver_table[DRIVER_COUNT];
// [/tag]

// do not edit code between tag : automatic generated code !
// [tag] task alloc
#define TASK_COUNT 2
task_table_t task_table[TASK_COUNT];
uint8_t task_current=0;
// [/tag]

int main(void) 
{
	// do not edit code between tag : automatic generated code !
	// [tag] task init
	uint8_t i=0; 
	taskCreate(task1,i++);
	taskCreate(task2,i++);
	// [/tag]

	
	// do not edit code between tag : automatic generated code !
	// [tag] driver init
	driver_table[0]=(driver_table_t) 
	{
		.driver_id = 0,
		.driver_name = timer1GetName(),
		.setStatus = timer1SetStatus, 
		.getStatus = timer1GetStatus, 
		.init = timer1Init, 
		.start = timer1Start, 
		.stop = timer1Stop
	};
	driver_table[1]=(driver_table_t) 
	{
		.driver_id = 1,
		.driver_name = timer3GetName(),
		.setStatus = timer3SetStatus, 
		.getStatus = timer3GetStatus, 
		.init = timer3Init, 
		.start = timer3Start, 
		.stop = timer3Stop
	};
	// [/tag]
	
	// driver flag init 
	for(i=0;i<DRIVER_COUNT;i++)
	{
		(*driver_table[i].setStatus)( (1 << DRIVER_INIT_AT_BOOT) | (1 << DRIVER_START_AT_BOOT) );
	}
	
	// init driver if flag on
	for(i=0;i<DRIVER_COUNT;i++)
	{
		if( ((*driver_table[i].getStatus)() & (1 << DRIVER_INIT_AT_BOOT) ) != 0 ){(*driver_table[i].init)();}
	}
	
	// start driver if flag on
	for(i=0;i<DRIVER_COUNT;i++)
	{
		if( ((*driver_table[i].getStatus)() & (1 << DRIVER_START_AT_BOOT) ) != 0 ){(*driver_table[i].start)();}
	}	
	
	
	//jump to current task for first call and start system
	task_current=0;
	SP = (uint16_t)task_table[task_current].stack_pointer;
	asm volatile (
		POP_ALL_REGS
		"sei \n\t"
		"ret \n\t");
	
	return 0;
}


ISR(TIMER3_COMPA_vect) 
{
	// RTC decrement
	for(uint8_t i=0;i<TASK_COUNT;i++)
	{
		if( task_table[i].task_RTC > 0 ){task_table[i].task_RTC--;}
	}
}

ISR(TIMER1_COMPA_vect, ISR_NAKED) 
{
	// enable global INT to catch RTC INT without delay
	sei();

	// Save current task context
	asm volatile ( PUSH_ALL_REGS );
	task_table[task_current].stack_pointer=(uint8_t *)SP;
	
	// switch context
	if(++task_current==TASK_COUNT){task_current=0;}
	
	// I'm alive blink in board led 13
	static uint8_t alive_cnt=0;
	if(++alive_cnt>250)
	{
		LED_PORT ^= (1 << LED_PIN);
		alive_cnt=0;
	}
	
	// Restore next task context
	SP = (uint16_t)task_table[task_current].stack_pointer;
	asm volatile ( POP_ALL_REGS "reti \n\t");
}
