#ifndef TASKMATE_DEFINE_H
#define TASKMATE_DEFINE_H

//in board LED 13 
#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_PIN PB7

#define PUSH_ALL_REGS "push r0 \n\t" \
		"in r0, __SREG__ \n\t" \
		"push r0 \n\t" \
		"push r1 \n\t" \
		"push r2 \n\t" \
		"push r3 \n\t" \
		"push r4 \n\t" \
		"push r5 \n\t" \
		"push r6 \n\t" \
		"push r7 \n\t" \
		"push r8 \n\t" \
		"push r9 \n\t" \
		"push r10 \n\t" \
		"push r11 \n\t" \
		"push r12 \n\t" \
		"push r13 \n\t" \
		"push r14 \n\t" \
		"push r15 \n\t" \
		"push r16 \n\t" \
		"push r17 \n\t" \
		"push r18 \n\t" \
		"push r19 \n\t" \
		"push r20 \n\t" \
		"push r21 \n\t" \
		"push r22 \n\t" \
		"push r23 \n\t" \
		"push r24 \n\t" \
		"push r25 \n\t" \
		"push r26 \n\t" \
		"push r27 \n\t" \
		"push r28 \n\t" \
		"push r29 \n\t" \
		"push r30 \n\t" \
		"push r31 \n\t"

#define POP_ALL_REGS "pop r31 \n\t" \
		"pop r30 \n\t" \
		"pop r29 \n\t" \
		"pop r28 \n\t" \
		"pop r27 \n\t" \
		"pop r26 \n\t" \
		"pop r25 \n\t" \
		"pop r24 \n\t" \
		"pop r23 \n\t" \
		"pop r22 \n\t" \
		"pop r21 \n\t" \
		"pop r20 \n\t" \
		"pop r19 \n\t" \
		"pop r18 \n\t" \
		"pop r17 \n\t" \
		"pop r16 \n\t" \
		"pop r15 \n\t" \
		"pop r14 \n\t" \
		"pop r13 \n\t" \
		"pop r12 \n\t" \
		"pop r11 \n\t" \
		"pop r10 \n\t" \
		"pop r9 \n\t" \
		"pop r8 \n\t" \
		"pop r7 \n\t" \
		"pop r6 \n\t" \
		"pop r5 \n\t" \
		"pop r4 \n\t" \
		"pop r3 \n\t" \
		"pop r2 \n\t" \
		"pop r1 \n\t" \
		"pop r0 \n\t" \
		"out __SREG__, r0 \n\t" \
		"pop r0	\n\t"

// task table
#define TASK_STACK_SIZE 256

typedef struct 
{
	uint8_t task_id;
	volatile uint16_t task_RTC;
	uint8_t *stack_pointer;
	uint8_t stack[TASK_STACK_SIZE];
} task_table_t;

// driver frame layout
#define FALSE 0
#define TRUE 1

#define SYSTEM_CORE_ID 0xff

// driver status bits
#define DRIVER_INIT_AT_BOOT 	0
#define DRIVER_START_AT_BOOT 	1
#define DRIVER_LOCK 			2
#define DRIVER_DEAD				3
#define DRIVER_INIT				4


typedef struct
{
	uint8_t driver_id;
	uint8_t *driver_name;
	
	void (*setStatus)(uint8_t);
	uint8_t (*getStatus)(void);
	
	void (*init)(void);
	void (*start)(void);
	void (*stop)(void);
} driver_table_t;

#endif
