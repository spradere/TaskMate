#include <avr/io.h>
#include <avr/interrupt.h>
#include "TaskMate0.2_private.h"
#include "TaskMate0.2_public.h"
#include "task1.h"
#include "task2.h"



int main(void) 
{
	// Create tasks
	taskCreate(task1, 0);
	taskCreate(task2, 1);
	
	
	// Set up timer1 interrupt for scheduler
	TCCR1B |= (1 << WGM12) | (1 << CS11); // CTC mode, prescaler 8
	OCR1A = 4000; // Interrupt every 2ms
	TIMSK1 |= (1 << OCIE1A);
	// Set output for in board led 13
	LED_DDR |= (1 << LED_PIN);
	
	/*// Set up timer3 interrupt for RTC
	TCCR3B |= (1 << WGM32) | (1 << CS31); // CTC mode, prescaler 8
	OCR3A = 2000; // Interrupt every 1ms
	TIMSK3 |= (1 << OCIE3A);*/	

	//jump to current task for first call
	SP = (uint16_t)task_table[task_current].stack_pointer;
	asm volatile (
		POP_R31_R0
		"out __SREG__, r0  \n\t"
		"pop r0            \n\t"
		"sei \n\t"
		"ret \n\t");
	
	while(1);
	return 0;
}

void taskCreate(void (*taskFunction)(void), uint8_t task_id)
{
	task_table[task_id].task_id=task_id;
	
	task_table[task_id].task_RTC=0;
	
	task_table[task_id].stack_pointer = &task_table[task_id].stack[TASK_STACK_SIZE - 1];
	*(task_table[task_id].stack_pointer--) = (uint16_t)taskFunction & 0xFF; //PCL; 
	*(task_table[task_id].stack_pointer--) = (uint16_t)taskFunction >> 8; //PCH
	*(task_table[task_id].stack_pointer--) = 0x00; //PCHH allways O if flash used < 64k
	*(task_table[task_id].stack_pointer--) = 0x00; //R0                        
	*(task_table[task_id].stack_pointer--) = SREG;                        
	
	// Registers R1-R31
	for (int i = 1; i < 32; i++) 
	{*(task_table[task_id].stack_pointer--) = 0x00;}
}	

ISR(TIMER3_COMPA_vect, ISR_NAKED) 
{
	/*// RTC timing
	for(int i=0;i<TASK_COUNT;i++)
	{
		if(task_table[i].task_RTC > 0){task_table[i].task_RTC--;}
	}*/
}

ISR(TIMER1_COMPA_vect, ISR_NAKED) 
{
	// enable global INT to catch RTC INT without delay
	//sei();

	// Save current task context
	asm volatile (
		"push r0          \n\t"
		"in r0, __SREG__  \n\t"
		PUSH_R0_R31 );
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

	// reset timer1 counter in case of cooperative call
	//TCNT1H=0;
	//TCNT1L=0;
	
	// Restore next task context
	SP = (uint16_t)task_table[task_current].stack_pointer;
	asm volatile (
		POP_R31_R0
		"out __SREG__, r0  \n\t"
		"pop r0            \n\t"
		"reti \n\t");
}
