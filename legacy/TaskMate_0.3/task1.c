#include <avr/io.h>
#include "TaskMate0.3_public.h"
#include "task1.h"

void task1(void)
{
    // Set PA0 as an output
    DDRA |= (1 << PA0);

    while (1) 
    {
        PORTA ^= (1 << PA0);
        sysCallSetTaskRTC(50);
        while(sysCallGetTaskRTC()>0);
    }

    return;
}
