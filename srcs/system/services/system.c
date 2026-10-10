/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file system.c
 * @brief system management implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "system.h"

#include <stdbool.h>
#include <stdint.h>

#include "interfaces/tm_driver_have.h"
#include "interfaces/tm_info.h"
#include "interfaces/tm_runLevel.h"
#include "system/sysCall/sc_driver.h"
#include "system/sysCall/sc_errors.h"
#include "system/sysCall/sc_gpio.h"
#include "system/sysCall/sc_threads.h"
#ifdef TM_DRIVER_HAVE_LCD
	#include "tmLibc/tm_stdio.h"
#endif
#include "tmLibc/tm_string.h"
#include "tmLibc/tm_syslog.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define SYSTEM_RUN_LEVEL_RR_ROUND_COUNT 10u
#define SYSTEM_IDLE_STC_TICKS 1u
#define SYSTEM_DISPLAY_STC_TICKS 50u
#define SYSTEM_SIZE_LCDMESSAGE 30U

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void systemStart(void);
static void systemRunLevelStart(tm_run_level_t run_level);
static bool systemRunLevelIsReady(tm_run_level_t run_level);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void system(void)
{
	// Finish staged startup before reporting system information.
	sc_gpio_signalInit();
	sc_threadSetInitialized();
	systemStart();

	// Print system info
	tm_syslog(
		TM_STR("[system] TaskMate v%u.%u build : %u\n"), TM_VER_MAJOR, TM_VER_MINOR, TM_BUILD);

#ifdef TM_DRIVER_HAVE_RTC
	hal_rtc_time_t time;
	sc_rtcRead(&time);
	tm_syslog(TM_STR("[system] date & time : %02u/%02u/20%02u %02u:%02u\n"),
			  time.day,
			  time.month,
			  time.year,
			  time.hours,
			  time.minutes);
#endif

#ifdef TM_DRIVER_HAVE_LCD
	char msg[SYSTEM_SIZE_LCDMESSAGE];
	tm_snprintf(
		msg, sizeof(msg), TM_STR("TaskMate %u.%u %u"), TM_VER_MAJOR, TM_VER_MINOR, TM_BUILD);
	sc_lcdClear();
	sc_lcdWriteString(TM_STR_RAM(msg), 0U, 0U);
#endif

	// After startup, refresh the display when available and yield between updates.
	while( 1U )
	{
#if defined(TM_DRIVER_HAVE_RTC) && defined(TM_DRIVER_HAVE_LCD)
		sc_rtcRead(&time);
		tm_snprintf(msg,
					sizeof(msg),
					TM_STR("%02u/%02u/20%02u %02u:%02u:%02u"),
					time.day,
					time.month,
					time.year,
					time.hours,
					time.minutes,
					time.seconds);
		sc_lcdWriteString(TM_STR_RAM(msg), 1U, 0U);
		sc_threadSetSTC(SYSTEM_DISPLAY_STC_TICKS);
#else
		sc_threadSetSTC(SYSTEM_IDLE_STC_TICKS);
#endif

		while( sc_threadGetSTC() > 0U ) { sc_coopYield(); };
	}
}

/* -----------------------------------------------
 * System startup
 * ---------------------------------------------*/

/*
 * Advance each run level after a complete scheduling round confirms its drivers and
 * threads are ready. Halt if readiness does not arrive within the fixed round limit.
 */
static void systemStart(void)
{
	tm_run_level_t run_level = sc_runLevelGet();
	uint8_t incomplete_round_count = 0U;

	if( run_level != RL_RUN_CORE ) { sc_halt(); }
	systemRunLevelStart(run_level);

	while( 1U )
	{
		/* As thread zero, returning here means one complete round-robin turn elapsed. */
		sc_coopYield();

		if( systemRunLevelIsReady(run_level) )
		{
			if( run_level == RL_RUN_USER ) { return; }

			run_level = (tm_run_level_t)(run_level + 1U);
			if( !sc_runLevelSet(run_level) ) { sc_halt(); }
			systemRunLevelStart(run_level);
			incomplete_round_count = 0U;
		}
		else
		{
			incomplete_round_count++;
			if( incomplete_round_count >= SYSTEM_RUN_LEVEL_RR_ROUND_COUNT ) { sc_halt(); }
		}
	}
}

static void systemRunLevelStart(tm_run_level_t run_level)
{
	tm_syslog(TM_STR("[system] switch to run level %u\n"), run_level);
	sc_driverRunLevelStart(run_level);

#ifdef TM_DRIVER_HAVE_I2C
	if( run_level == RL_RUN_CORE ) { (void)sc_i2cScan(); }
#endif
#ifdef TM_DRIVER_HAVE_RTC
	if( run_level == RL_RUN_DRIVER ) { (void)sc_rtcSaveStartupTime(); }
#endif
}

static bool systemRunLevelIsReady(tm_run_level_t run_level)
{
	for( tm_run_level_t level = RL_RUN_NONE; level <= run_level;
		 level = (tm_run_level_t)(level + 1U) )
	{
		if( !sc_driverRunLevelIsReady(level) ) { return false; }
	}

	for( tm_run_level_t level = RL_RUN_CORE; level <= run_level;
		 level = (tm_run_level_t)(level + 1U) )
	{
		if( !sc_threadRunLevelIsReady(level) ) { return false; }
	}

	return true;
}
