#include "TaskMate.h"
#include "task1.h"
#include "task2.h"


volatile uint16_t timer1_cnt=0;

typedef struct {
    uint8_t *stack_pointer;
    uint8_t stack[STACK_SIZE];
} Task;

Task tasks_table[TASKS_COUNT];
volatile int8_t current_task = 0;

void createTask(void (*taskFunction)(void), uint8_t task_id);


int main(void) 
{
	 // Create tasks
    createTask(task1, 0);
    createTask(task2, 1);
    
    
	// Set up timer interrupt for scheduler
    TCCR1B |= (1 << WGM12) | (1 << CS11); // CTC mode, prescaler 8
    OCR1A = 2000; // Interrupt every 1ms
    TIMSK1 |= (1 << OCIE1A);
    // Set PB7 as an output, in board led 13
    DDRB |= (1 << PB7);
    
    //sei(); // Enable global interrupts
    
    //jump to current task for first call
    SP = (uint16_t)tasks_table[current_task].stack_pointer;
    asm volatile (
       "POP R31\n\t"         
        "POP R30\n\t"
        "POP R29\n\t"
        "POP R28\n\t"
        "POP R27\n\t"
        "POP R26\n\t"
        "POP R25\n\t"
        "POP R24\n\t"
        "POP R23\n\t"
        "POP R22\n\t"
        "POP R21\n\t"
        "POP R20\n\t"
        "POP R19\n\t"
        "POP R18\n\t"
        "POP R17\n\t"
        "POP R16\n\t"
        "POP R15\n\t"
        "POP R14\n\t"
        "POP R13\n\t"
        "POP R12\n\t"
        "POP R11\n\t"
        "POP R10\n\t"
        "POP R9\n\t"
        "POP R8\n\t"
        "POP R7\n\t"
        "POP R6\n\t"
        "POP R5\n\t"
        "POP R4\n\t"
        "POP R3\n\t"
        "POP R2\n\t"
        "pop r1        \n\t"
		"pop r0            \n\t"
        "out __SREG__, r0  \n\t"
        "pop r0            \n\t"
        "sei \n\t"
        "ret \n\t");
    
	while(1);
	
    return 0;
}

void createTask(void (*taskFunction)(void), uint8_t task_id)
{
	tasks_table[task_id].stack_pointer = &tasks_table[task_id].stack[STACK_SIZE - 1];
    *(tasks_table[task_id].stack_pointer--) = (uint16_t)taskFunction & 0xFF; //PCL; 
    *(tasks_table[task_id].stack_pointer--) = (uint16_t)taskFunction >> 8; //PCH
    *(tasks_table[task_id].stack_pointer--) = 0x00; //PCHH allways O if flash used < 64k
    *(tasks_table[task_id].stack_pointer--) = 0x00; //R0                        
    *(tasks_table[task_id].stack_pointer--) = SREG;                        
    for (int i = 1; i < 32; i++) 
    {                                  
		// Registers R1-R31
        *(tasks_table[task_id].stack_pointer--) = 0x00;
    }
}	



ISR(TIMER1_COMPA_vect, ISR_NAKED) 
{
	// I'm alive 
	timer1_cnt++;
	if(timer1_cnt > 500)
	{
		PORTB ^= (1 << PB7);
		timer1_cnt=0;
	}

	
	// Save current task context
	asm volatile (
		"push r0          \n\t"
		"in r0, __SREG__  \n\t"
		"push r0          \n\t"
	    "PUSH R1\n\t"
		"PUSH R2\n\t"
		"PUSH R3\n\t"
		"PUSH R4\n\t"
		"PUSH R5\n\t"
		"PUSH R6\n\t"
		"PUSH R7\n\t"
		"PUSH R8\n\t"
		"PUSH R9\n\t"
		"PUSH R10\n\t"
		"PUSH R11\n\t"
		"PUSH R12\n\t"
		"PUSH R13\n\t"
		"PUSH R14\n\t"
		"PUSH R15\n\t"
		"PUSH R16\n\t"
		"PUSH R17\n\t"
		"PUSH R18\n\t"
		"PUSH R19\n\t"
		"PUSH R20\n\t"
		"PUSH R21\n\t"
		"PUSH R22\n\t"
		"PUSH R23\n\t"
		"PUSH R24\n\t"
		"PUSH R25\n\t"
		"PUSH R26\n\t"
		"PUSH R27\n\t"
		"PUSH R28\n\t"
		"PUSH R29\n\t"	
		"PUSH R30\n\t"
		"PUSH R31\n\t");
	
	tasks_table[current_task].stack_pointer=(uint8_t *)SP;
	
	current_task++; if(current_task>1){current_task=0;}
	
	SP = (uint16_t)tasks_table[current_task].stack_pointer;
	
	// Restore next task context
    asm volatile (
        "POP R31\n\t"         
        "POP R30\n\t"
        "POP R29\n\t"
        "POP R28\n\t"
        "POP R27\n\t"
        "POP R26\n\t"
        "POP R25\n\t"
        "POP R24\n\t"
        "POP R23\n\t"
        "POP R22\n\t"
        "POP R21\n\t"
        "POP R20\n\t"
        "POP R19\n\t"
        "POP R18\n\t"
        "POP R17\n\t"
        "POP R16\n\t"
        "POP R15\n\t"
        "POP R14\n\t"
        "POP R13\n\t"
        "POP R12\n\t"
        "POP R11\n\t"
        "POP R10\n\t"
        "POP R9\n\t"
        "POP R8\n\t"
        "POP R7\n\t"
        "POP R6\n\t"
        "POP R5\n\t"
        "POP R4\n\t"
        "POP R3\n\t"
        "POP R2\n\t"
        "pop r1        \n\t"
        "pop r0            \n\t"
        "out __SREG__, r0  \n\t"
        "pop r0            \n\t"
        "reti \n\t");
}
