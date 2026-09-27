/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file ucontext_system.c
 * @brief Minimal system service for the ucontext simulation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "system/services/system.h"

#include <stdbool.h>
#include <stdint.h>

#include "interfaces/tm_info.h"
#include "interfaces/tm_runLevel.h"
#include "system/sysCall/sc_driver.h"
#include "system/sysCall/sc_errors.h"
#include "system/sysCall/sc_gpio.h"
#include "system/sysCall/sc_threads.h"
#include "tmLibc/tm_string.h"
#include "tmLibc/tm_syslog.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define UCONTEXT_SYSTEM_RUN_LEVEL_RR_ROUND_COUNT 10u

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void systemStart(void);
static void systemRunLevelStart(uint8_t run_level);
static bool systemRunLevelIsReady(uint8_t run_level);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void system(void)
{
	sc_gpio_signalInit();
	sc_threadSetInitialized();
	systemStart();

	tm_syslog(TM_STR("[system] TaskMate v%u.%u build %u - FreeBSD/ucontext\n"),
			  TM_VER_MAJOR,
			  TM_VER_MINOR,
			  TM_BUILD);

	while( 1 )
	{
		sc_threadSetSTC(1);
		while( sc_threadGetSTC() > 0 ) { sc_coopYield(); }
	}
}

static void systemStart(void)
{
	uint8_t run_level = sc_runLevelGet();
	uint8_t incomplete_round_count = 0;

	if( run_level != RL_RUN_CORE ) { sc_halt(); }
	systemRunLevelStart(run_level);

	while( 1 )
	{
		sc_coopYield();
		if( systemRunLevelIsReady(run_level) )
		{
			if( run_level == RL_RUN_USER ) { return; }
			run_level++;
			if( !sc_runLevelSet(run_level) ) { sc_halt(); }
			systemRunLevelStart(run_level);
			incomplete_round_count = 0;
		}
		else
		{
			incomplete_round_count++;
			if( incomplete_round_count >= UCONTEXT_SYSTEM_RUN_LEVEL_RR_ROUND_COUNT )
			{
				sc_halt();
			}
		}
	}
}

static void systemRunLevelStart(uint8_t run_level)
{
	tm_syslog(TM_STR("[system] switch to run level %u\n"), run_level);
	sc_driverRunLevelStart(run_level);
}

static bool systemRunLevelIsReady(uint8_t run_level)
{
	for( uint8_t level = RL_RUN_NONE; level <= run_level; level++ )
	{
		if( !sc_driverRunLevelIsReady(level) ) { return false; }
	}
	for( uint8_t level = RL_RUN_CORE; level <= run_level; level++ )
	{
		if( !sc_threadRunLevelIsReady(level) ) { return false; }
	}
	return true;
}
