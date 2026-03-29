/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file boot.c
 * @brief boot start up
 *
 */

#include "sysCore/boot.h"

#include "hal/public/auto_hal_init.h"
#include "hal/public/hal_usart.h"
#include "sysCore/gpio.h"
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

	// start driver
	// TODO remove this code when run level is implemented

	for( uint8_t i = 0; i < MOD_DRIVER_COUNT; i++ )
	{
		mod_driver_item_t *mod = mod_driverGetPointer(i);
		(*(mod->init))();
		(*(mod->start))();
	}
}
