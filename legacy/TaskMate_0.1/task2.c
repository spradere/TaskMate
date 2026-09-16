#include "TaskMate.h"
#include "task2.h"

void task2(void)
{
    // Set PA1 as an output
    DDRA |= (1 << PA1);

    while (1) {
		uint8_t p=PORTA & 0x01;
		
        if(p==0){PORTA &= 0xFE;}
        else {PORTA |= 0x01;}
        }

    return;
}
