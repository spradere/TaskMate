/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_scli_index.c
 * @brief Exercise bounded index parsing in SCLI commands.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <assert.h>
#include <string.h>

#include "interfaces/tm_string.h"

/* Test-only RAM storage keeps the service sources independent of a target backend. */
#define HAL_STRING_RAM(string) \
	((tm_string_t){.text = (const uint8_t *)(string), .storage = TM_MEM_RAM})
#define HAL_STRING_ROM(string) HAL_STRING_RAM(string)
#define HAL_STRING(string) HAL_STRING_RAM(string)
#define HAL_STRING_INROM(name, txt) static const char name[] = (txt)

#define SCLITEST_MAX_DAY 31U

#include "system/services/scli.c"
#include "system/services/commands/scli_thread.c"
#include "system/services/commands/scli_date.c"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

int main(void)
{
	char *argv[SCLI_ARGUMENT_COUNT_MAX];
	tm_run_level_t run_level = RL_RUN_NONE;
	hal_rtc_time_t time = {0};

	strcpy(scli_line, " thread start task ");
	assert(scliTokenize(argv) == SCLI_ARGUMENT_COUNT_MAX - 1U);
	assert(strcmp(argv[0U], "thread") == 0);
	assert(strcmp(argv[1U], "start") == 0);
	assert(strcmp(argv[2U], "task") == 0);

	strcpy(scli_line, "  \t ");
	assert(scliTokenize(argv) == 0U);
	strcpy(scli_line, "one two three four five");
	assert(scliTokenize(argv) == 0U);
	memset(scli_line, 'x', sizeof(scli_line));
	assert(scliTokenize(argv) == 0U);
	scli_line[sizeof(scli_line) - 1U] = 0U;
	assert(scliTokenize(argv) == 1U);

	assert(threadRunLevelParse("1", &run_level));
	assert(run_level == RL_RUN_CORE);
	assert(threadRunLevelParse("4", &run_level));
	assert(run_level == RL_RUN_USER);
	assert(!threadRunLevelParse("0", &run_level));
	assert(!threadRunLevelParse("5", &run_level));
	assert(!threadRunLevelParse("255", &run_level));
	assert(!threadRunLevelParse("256", &run_level));
	assert(!threadRunLevelParse("1234", &run_level));
	assert(!threadRunLevelParse("1x", &run_level));
	assert(!threadRunLevelParse("", &run_level));

	assert(dateParseTime("23:59:59", &time));
	assert(time.hours == SCLIDATE_MAX_HOURS);
	assert(!dateParseTime("24:00:00", &time));
	assert(!dateParseTime("23:59:590", &time));
	assert(dateParseDate("31/12/2099", &time));
	assert(time.day == SCLITEST_MAX_DAY);
	assert(!dateParseDate("31/12/2100", &time));
	assert(!dateParseDate("31/12/20999", &time));

	return 0;
}
