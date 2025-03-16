/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

 /**
 * @file scli.c
 * @brief implementation of serial command line interface
 * 
 * 
 */
 
#include <avr/io.h>

#include "sysCore/TaskMate_public.h"
#include "drivers/usart1.h"
#include "tasks/scli.h"
#include "tasks/lcd.h"


void scli(void)
{
	while(1)
	{
		scliEcho(); // Echo echo echo echo echo echo echo
		sysCallSetTaskRTC(100);
        while(sysCallGetTaskRTC()>0);
	}
	
	return;
}

void scliEcho(void) 
{
    uint8_t data;
    uint8_t satus = usart1Read(&data);
    
        
    lcdSetCursor(1,0);
    
    if( satus== 0) // If data available
    {  
		usart1Write(data);  // Echo received character
		lcdWriteString("data recived");
    }
    else
    {
		lcdWriteString("no USART data     ");
	}
	
}
