/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file boot.c
 * @brief sysCore boot implementation.
 *
 */

#include "sysCore/boot.h"

#include "hal/public/hal_i2c.h"
#include "hal/public/hal_usart.h"
#include "interfaces/runLevel_define.h"
#include "sysCore/gpio.h"
#include "sysCore/hal_init.h"
#include "sysCore/modules.h"
#include "sysCore/runLevel.h"
#include "tm_libc/tm_syslog.h"

void boot(void)
{
	// system startup
	hal_usartInit();
	hal_usartStart();
	tm_syslog(TM_STR("\n\n[boot] booting ...\n"));

	// system static allocation init
	tm_syslog(TM_STR("[boot] system static allocation\n"));

	mod_driversAlloc();
	mod_threadsAlloc();
	rl_Alloc();

	// hal hardware init
	tm_syslog(TM_STR("[boot] hal hardware init\n"));

	hal_archInit();
	hal_mcuInit();
	hal_boardInit();
	gpio_signalsInit();

	// start drivers
	for( uint8_t runlevel = 1; runlevel < RUN_LEVEL_COUNT; runlevel++ )
	{
		for( uint8_t i = 0; i < TM_MOD_DRIVER_COUNT; i++ )
		{
			mod_driver_item_t *mod = mod_driverGetPointer(i);
			if( (mod->status & RUN_LEVEL_MASK) == runlevel )
			{
				(*(mod->init))();
				(*(mod->start))();
			}
		}
	}
}
