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
 * @todo Implement commands, before develop error handler
 */

#include <avr/io.h>

#include "sysCore/TaskMate_public.h"
#include "drivers/usart1.h"
#include "tasks/scli.h"
#include "tasks/lcd.h"


// status
uint8_t scli_status = 0;

void scliSetStatus(uint8_t status) { scli_status = status; }
uint8_t scliGetStatus(void) { return scli_status; }


void scli(void)
{
	while (1)
	{
		scliEcho(); // Echo echo echo echo echo echo echo
		sysCallSetTaskRTC(100);
		while (sysCallGetTaskRTC() > 0);
	}

	return;
}

void scliEcho(void)
{
	uint8_t data;

	if (usart1TestBufferRx() != ERR_USART_RX_BUFFER_EMPTY)
	{
		lcdSetCursor(1, 0);
		lcdWriteString("USART data in  ");

		usart1WriteString("scli.c : ");

		while (usart1Read(&data) != ERR_USART_RX_BUFFER_EMPTY)
		{
			if (usart1Write(data) == ERR_USART_TX_BUFFER_FULL)
			{
				break;
			}
		}
		usart1Flush(); // write all Tx buffer to usart
	}
	else
	{
		lcdSetCursor(1, 0);
		lcdWriteString("USART no data  ");
	}
}
