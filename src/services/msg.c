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
 * @todo all
 */

 #include <avr/io.h>

#include "sysCore/TaskMate_public.h"
#include "services/msg.h"

// variables
channel_item_t channels[MSG_CHANNELS_MAX];


void msg(void)
{
	// init
	for(uint8_t i = 0; i < MSG_CHANNELS_MAX; i++)
		{channels[i].status = 0; }

	//run
	while( 1 )
	{
		msgManageDisplay();

		sysCallSetThreadTC(10);
		while( sysCallGetThreadTC() > 0 ) { sysCallYieldHand(); };
	}
}

errorCode_t msgRequestChannel(uint8_t *channel_id)
{
	for(uint8_t i = 0; i < MSG_CHANNELS_MAX; i++)
	{
		if( (channels[i].status & (1 << MSG_FLAG_IN_USE)) == 0)
		{
			channels[i].status &= (1 << MSG_FLAG_IN_USE);
			*channel_id = i;
			return ERR_SUCCESS;
		}
	}
	return ERR_MSG_OUT_OF_FREE_CHANNEL;
}

void msgManageDisplay(void)
{
}

