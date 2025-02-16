#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "sysCore/TaskMate_define.h"
#include "sysCore/taskCreate.h"

#include "drivers/timer1.h"
#include "drivers/timer3.h"

#include "tasks/task1.h"
#include "tasks/task2.h"


// task init
#define TASK_COUNT 2
task_t task_table[TASK_COUNT];
int8_t task_current = 0;



int main(void) 
{
	// Create tasks
	uint8_t ID=0; 
	taskCreate(task1,ID++);
	taskCreate(task2,ID++);
	
	timer1Init();
	timer1Lock(SYSTEM_CORE_ID);

	timer3Init();
	timer3Lock(SYSTEM_CORE_ID);

	
	//jump to current task for first call and run system
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
