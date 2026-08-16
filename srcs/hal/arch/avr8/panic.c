/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file panic.c
 * @brief panic implementation
 *
 */

#include "panic.h"

#include "interrupt.h"
#include "hal/mcu/atmega2560/usart.h"

void panic(const char *msg)
{
	hal_clearGlobalInterupt();
	hal_usartSendTXBuffer();                   
	hal_usartWriteString("\nSYSTEM PANIC : "); 
	hal_usartSendTXBuffer();                   
	hal_usartWriteString(msg);                 
	hal_usartSendTXBuffer();                   
	hal_usartWriteString(", halt.");           
	hal_usartSendTXBuffer();                   
	while( 1 );                                
}

// the end, really.
