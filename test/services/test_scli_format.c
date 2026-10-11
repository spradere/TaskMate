/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_scli_format.c
 * @brief Verify SCLI command output through the TaskMate formatter.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "hal/host/ucontext/ucontext_string_macro.h"
#include "tmLibc/stdio/tm_snprintf.h"

#include "system/services/commands/scli_driver.c"
#include "system/services/commands/scli_thread.c"
#include "system/services/commands/scli_date.c"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define TEST_SCLI_OUTPUT_SIZE 200U
#define TEST_SCLI_ERROR_CODE 42U

/* -----------------------------------------------
 * Test state
 * ---------------------------------------------*/

static char test_output[TEST_SCLI_OUTPUT_SIZE];
static uint8_t test_output_length;
static const tm_string_t test_driver_name = {
	.text = (const uint8_t *)"timerContext", .storage = TM_MEM_RAM};
static const tm_string_t test_thread_name = {
	.text = (const uint8_t *)"task1", .storage = TM_MEM_RAM};

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void testResetOutput(void);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Syscall and logging dependencies
 * ---------------------------------------------*/

uint8_t sc_stringGetByte(const tm_string_t *string, uint8_t index)
{
	return string->text[index];
}

err_codes_t sc_consoleWriteByte(uint8_t data)
{
	(void)data;
	return ERR_NO_ERROR;
}

err_codes_t sc_consoleFlush(void) { return ERR_NO_ERROR; }

void tm_syslog(tm_string_t format, ...)
{
	va_list args;
	va_start(args, format);
	int written = tm_vsnprintf(&test_output[test_output_length],
		(uint8_t)(TEST_SCLI_OUTPUT_SIZE - test_output_length), format, args);
	va_end(args);
	assert(written >= 0);
	assert(written < (int)(TEST_SCLI_OUTPUT_SIZE - test_output_length));
	test_output_length += (uint8_t)written;
}

mod_count_t sc_driverGetCount(void) { return 1U; }

bool sc_driverGetInfo(mod_count_t id, const tm_string_t **name, tm_run_level_t *run_level,
					  hal_driver_status_t *status_bits)
{
	assert(id == 0U);
	*name = &test_driver_name;
	*run_level = RL_RUN_DRIVER;
	*status_bits = (hal_driver_status_t)((1U << DRV_BIT_INIT) | (1U << DRV_BIT_START));
	return true;
}

err_code_t sc_driverInit(const char *name)
{
	(void)name;
	return TEST_SCLI_ERROR_CODE;
}

err_code_t sc_driverStart(const char *name)
{
	(void)name;
	return TEST_SCLI_ERROR_CODE;
}

err_code_t sc_driverStop(const char *name)
{
	(void)name;
	return TEST_SCLI_ERROR_CODE;
}

const tm_string_t *err_getMessage(err_codes_t code)
{
	(void)code;
	return NULL;
}

mod_count_t sc_threadGetCount(void) { return 1U; }

bool sc_threadGetInfo(mod_count_t id, const tm_string_t **name, tm_run_level_t *run_level,
					  uint16_t *stack_size_bytes)
{
	assert(id == 0U);
	*name = &test_thread_name;
	*run_level = RL_RUN_USER;
	*stack_size_bytes = 0U;
	return true;
}

/* -----------------------------------------------
 * Test cases
 * ---------------------------------------------*/

static void testResetOutput(void)
{
	test_output_length = 0U;
	test_output[0U] = 0U;
}

int main(void)
{
	char driver_name[] = "timerContext";
	char *driver_argv[] = {"driver", "init", driver_name};
	hal_rtc_time_t time = {.day = 1U, .month = 1U, .hours = 1U, .minutes = 1U,
		.seconds = 1U, .year = 0U};

	testResetOutput();
	assert(driverList(0U, NULL));
	assert(strcmp(test_output,
		"[driver] drivers:\n\ttimerContext runlevel=2 status[init=1 start=1 error=0 dead=0]\n") ==
		0);

	testResetOutput();
	assert(threadList(0U, NULL));
	assert(strcmp(test_output, "[thread] threads:\n\ttask1 runlevel=4\n") == 0);

	testResetOutput();
	datePrint(&time);
	assert(strcmp(test_output, "[date] 01/01/2000 01:01:01\n") == 0);

	testResetOutput();
	assert(!driverInit(SCLIDRIVER_COUNT_ACTIONARGS, driver_argv));
	assert(strcmp(test_output, "[driver] timerContext not initialized: error 42\n") == 0);

	driver_argv[1U] = "start";
	testResetOutput();
	assert(!driverStart(SCLIDRIVER_COUNT_ACTIONARGS, driver_argv));
	assert(strcmp(test_output, "[driver] timerContext not started: error 42\n") == 0);

	driver_argv[1U] = "stop";
	testResetOutput();
	assert(!driverStop(SCLIDRIVER_COUNT_ACTIONARGS, driver_argv));
	assert(strcmp(test_output, "[driver] timerContext not stopped: error 42\n") == 0);

	return 0;
}
