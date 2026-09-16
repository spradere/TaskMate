#include <avr/io.h>
#include <util/delay.h>
#include "TaskMate0.2_public.h"
#include "task2.h"

void task2(void)
{
    // Set PA1 as an output
    DDRA |= (1 << PA1);

    while (1) 
    {
		PORTA ^= (1 << PA1);
		_delay_ms(350); //be remplaced by a system call
    }

    return;
}
