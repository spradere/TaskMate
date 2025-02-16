#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "sysCore/TaskMate_private.h"
#include "tasks/task1.h"
#include "tasks/task2.h"


int main(void) 
{
	// Create tasks
	uint8_t ID=0;
	taskCreate(task1,ID++);
	taskCreate(task2,ID++);
	
	// Set up timer1 interrupt for scheduler
	TCCR1B |= (1 << WGM12) | (1 << CS11); // CTC mode, prescaler 8
	OCR1A = 1999; // Interrupt every 1ms
	TIMSK1 |= (1 << OCIE1A);
	// Set output for in board led 13
	LED_DDR |= (1 << LED_PIN);
	
	// Set up timer3 interrupt for RTC
	TCCR3B |= (1 << WGM32) | (1 << CS32); // CTC mode, prescaler 256
	OCR3A = 624; // Interrupt every 10ms
	TIMSK3 |= (1 << OCIE3A);
	
	//jump to current task for first call
	SP = (uint16_t)task_table[task_current].stack_pointer;
	asm volatile (
		POP_ALL_REGS
		"sei \n\t"
		"ret \n\t");
	
	return 0;
}

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
