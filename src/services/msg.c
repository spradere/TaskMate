/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file msg.c
 * @brief implementation of message display server
 *
 * @todo add free channel
 */

#include "services/msg.h"
#include "sysCall/sysCall.h"
#include "libc/string.h"
#include "libc/stdio.h"

// Send message to :
#include "hal/auto_hal_user.h"

// variables
channel_item_t channels[MSG_CHANNELS_MAX];

void msgProcess(void);

void msg(void)
{
	// init channels
	for( uint8_t i = 0; i < MSG_CHANNELS_MAX; i++ )
	{
		channels[i].status = 0;
		channels[i].text[0] = 0;
	}

	// run
	// hal_usartWriteString("[msg] stating message server\n");
	// hal_usartSendTXBuffer();

	uint8_t chan;

	if( msgRequestChannel(&chan) == ERR_NO_ERROR )
	{
		msgWritreText(chan, "[msg] canal test to USART\n", MSG_TO_USART);
	}

	msgProcess();
	msgFreeChannel(chan);

	if( msgRequestChannel(&chan) == ERR_NO_ERROR ) { msgWritreText(chan, "\1refactor modules 10", MSG_TO_LCD); }

	msgProcess();
	msgFreeChannel(chan);

	while( 1 )
	{
		sysCallSetThreadRTC(100);
		while( sysCallGetThreadRTC() > 0 ) { sysCallYieldHand(); };
		msgProcess();
	}
}

error_codes_t msgRequestChannel(uint8_t *channel)
{
	for( uint8_t i = 0; i < MSG_CHANNELS_MAX; i++ )
	{
		if( (channels[i].status & (uint8_t)(1u << MSG_FLAG_IN_USE)) == 0 )
		{
			channels[i].status |= (uint8_t)(1u << MSG_FLAG_IN_USE);
			*channel = i;
			return ERR_NO_ERROR;
		}
	}
	return ERR_MSG_OUT_OF_FREE_CHANNEL;
}

void msgFreeChannel(uint8_t channel) { channels[channel].status &= (uint8_t)~(1u << MSG_FLAG_IN_USE); }

void msgWritreText(uint8_t channel, const char *msg, uint8_t dest)
{
	// todo wait here MSG_FLAG_SEND to don't overwrite sessage
	// char debug[64];
	// uint8_t flag = channels[channel].status & (1 << MSG_FLAG_SEND);
	// snprintf(debug,64,"[msg:write] debug satus=%i flag=<%i>\n",channels[channel].status, flag );

	// hal_usartWriteString(debug);
	// hal_usartSendTXBuffer();

	/*if(flag == MSG_FLAG_SEND)
	{
		do
		{
			//sysCallYieldHand();
			flag = channels[channel].status & (1 << MSG_FLAG_SEND);
			//hal_usartWriteChar('#');
			//hal_usartSendTXBuffer();
		}
		while(flag == MSG_FLAG_SEND);
	}*/

	strncpy(channels[channel].text, MSG_SIZE_MAX, msg);

	channels[channel].status &= (uint8_t)~MSG_TO_MASK;
	channels[channel].status |= dest;
	channels[channel].status |= (uint8_t)(1u << MSG_FLAG_SEND);
}

void msgProcess(void)
{
	for( uint8_t channel = 0; channel < MSG_CHANNELS_MAX; channel++ )
	{
		if( ((channels[channel].status & (1 << MSG_FLAG_SEND)) != 0) &&
			((channels[channel].status & (1 << MSG_FLAG_IN_USE)) != 0) )
		{
			switch( channels[channel].status & MSG_TO_MASK )
			{
				case MSG_TO_LCD:

					hal_lcdSetCursor((uint8_t)channels[channel].text[0], 0);
					uint8_t i_src, i_dest = 0;
					// i_src = 1 zap escape code for LCD line select
					for( i_src = 1; channels[channel].text[i_dest] != 0; i_src++ )
					{
						channels[channel].text[i_dest++] = channels[channel].text[i_src];
					}
					channels[channel].text[i_dest] = 0;

					hal_lcdWriteString(channels[channel].text);
					break;

				case MSG_TO_USART:

					hal_usartWriteString(channels[channel].text);
					hal_usartSendTXBuffer();
					channels[channel].status &= (uint8_t)~(1u << MSG_FLAG_SEND);
					break;

				case MSG_TO_NULL:
					break;

				default:
					hal_usartWriteString("[msg.c:118] error unknow destination\n");
					hal_usartSendTXBuffer();
			}
		}
	}
}
