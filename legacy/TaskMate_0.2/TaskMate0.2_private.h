
#define TASK_COUNT 2
#define TASK_STACK_SIZE 256

#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_PIN PB7

#define PUSH_R0_R31 "PUSH R0\n\t" \
		"PUSH R1\n\t" \
		"PUSH R2\n\t" \
		"PUSH R3\n\t" \
		"PUSH R4\n\t" \
		"PUSH R5\n\t" \
		"PUSH R6\n\t" \
		"PUSH R7\n\t" \
		"PUSH R8\n\t" \
		"PUSH R9\n\t" \
		"PUSH R10\n\t" \
		"PUSH R11\n\t" \
		"PUSH R12\n\t" \
		"PUSH R13\n\t" \
		"PUSH R14\n\t" \
		"PUSH R15\n\t" \
		"PUSH R16\n\t" \
		"PUSH R17\n\t" \
		"PUSH R18\n\t" \
		"PUSH R19\n\t" \
		"PUSH R20\n\t" \
		"PUSH R21\n\t" \
		"PUSH R22\n\t" \
		"PUSH R23\n\t" \
		"PUSH R24\n\t" \
		"PUSH R25\n\t" \
		"PUSH R26\n\t" \
		"PUSH R27\n\t" \
		"PUSH R28\n\t" \
		"PUSH R29\n\t" \
		"PUSH R30\n\t" \
		"PUSH R31\n\t"

#define POP_R31_R0 "POP R31\n\t" \
		"POP R30\n\t" \
		"POP R29\n\t" \
		"POP R28\n\t" \
		"POP R27\n\t" \
		"POP R26\n\t" \
		"POP R25\n\t" \
		"POP R24\n\t" \
		"POP R23\n\t" \
		"POP R22\n\t" \
		"POP R21\n\t" \
		"POP R20\n\t" \
		"POP R19\n\t" \
		"POP R18\n\t" \
		"POP R17\n\t" \
		"POP R16\n\t" \
		"POP R15\n\t" \
		"POP R14\n\t" \
		"POP R13\n\t" \
		"POP R12\n\t" \
		"POP R11\n\t" \
		"POP R10\n\t" \
		"POP R9\n\t" \
		"POP R8\n\t" \
		"POP R7\n\t" \
		"POP R6\n\t" \
		"POP R5\n\t" \
		"POP R4\n\t" \
		"POP R3\n\t" \
		"POP R2\n\t" \
		"POP R1\n\t" \
		"POP R0\n\t" 

typedef struct {
	uint8_t task_id;
	uint16_t task_RTC;
	uint8_t *stack_pointer;
	uint8_t stack[TASK_STACK_SIZE];
} Task;

Task task_table[TASK_COUNT];
int8_t task_current = 0;

void taskCreate(void (*taskFunction)(void), uint8_t task_id);
