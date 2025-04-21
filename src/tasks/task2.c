#include <avr/io.h>
#include "sysCore/TaskMate_public.h"
#include "tasks/task2.h"

// status
uint8_t task2_status = 0;

void task2SetStatus(uint8_t status) { task2_status = status; }
uint8_t task2GetStatus(void) { return task2_status; }

void task2(void)
{
	// Set PA1 as an output
	DDRA |= (1 << PA1);

	while (1)
	{
		PORTA ^= (1 << PA1);
		sysCallSetThreadTC(50);
		while (sysCallGetThreadTC() > 0);
	}

	return;
}
