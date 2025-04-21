#include <avr/io.h>
#include "sysCore/TaskMate_public.h"
#include "tasks/task1.h"

// status
uint8_t task1_status = 0;

void task1SetStatus(uint8_t status) { task1_status = status; }
uint8_t task1GetStatus(void) { return task1_status; }

void task1(void)
{
	// Set PA0 as an output
	DDRA |= (1 << PA0);

	while (1)
	{
		PORTA ^= (1 << PA0);
		sysCallSetThreadTC(50);
		while (sysCallGetThreadTC() > 0);
	}

	return;
}
