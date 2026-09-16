#include <avr/io.h>
#include "TaskMate0.3_public.h"
#include "task2.h"

void task2(void)
{
    // Set PA1 as an output
    DDRA |= (1 << PA1);

    while (1) 
    {
		PORTA ^= (1 << PA1);
		sysCallSetTaskRTC(50);
        while(sysCallGetTaskRTC()>0);
    }

    return;
}
