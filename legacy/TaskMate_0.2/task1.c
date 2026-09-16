#include <avr/io.h>
#include <util/delay.h>
#include "TaskMate0.2_public.h"
#include "task1.h"

void task1(void)
{
    // Set PA0 as an output
    DDRA |= (1 << PA0);

    while (1) {
        PORTA ^= (1 << PA0);
        _delay_ms(100); //be remplaced by a system call
    }

    return;
}
