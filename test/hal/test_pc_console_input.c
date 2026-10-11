/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_pc_console_input.c
 * @brief Check buffered ncurses input and burst rendering.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <ncurses.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hal/board/pc/pc_console.h"
#include "interfaces/hal_halt.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define TESTCONSOLE_INPUT_ROW 22
#define TESTCONSOLE_INPUT_COLUMN 1
#define TESTCONSOLE_LINE_CAPACITY 64U
#define TESTCONSOLE_LINE_LIMIT 63U
#define TESTCONSOLE_LINE_SHORT 62U
#define TESTCONSOLE_PRINTABLE_COUNT 2U
#define TESTCONSOLE_KEY_DELETE 0x7F
#define TESTCONSOLE_KEY_HALT 10
#define TESTCONSOLE_HALT_STATUS 42

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static int refresh_count;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static bool queueKeys(const int *keys, size_t count);
static bool screenHasText(const char *text, size_t length);
static bool readLine(char expected, size_t length);
static bool checkBoundary(size_t typed, size_t accepted);
static bool checkStage(bool condition, const char *stage);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

int test_wrefresh(WINDOW *window)
{
	refresh_count++;
	return wrefresh(window);
}

_Noreturn void hal_halt(void) { exit(TESTCONSOLE_HALT_STATUS); }

static bool queueKeys(const int *keys, size_t count)
{
	while( count > 0U )
	{
		count--;
		if( ungetch(keys[count]) == ERR ) { return false; }
	}
	return true;
}

static bool screenHasText(const char *text, size_t length)
{
	int cursor_column = getcurx(curscr);
	for( size_t index = 0U; index < length; index++ )
	{
		int character = mvwinch(curscr, TESTCONSOLE_INPUT_ROW,
							   TESTCONSOLE_INPUT_COLUMN + (int)index) & A_CHARTEXT;
		if( character != text[index] ) { return false; }
	}
	return cursor_column == TESTCONSOLE_INPUT_COLUMN + (int)length;
}

static bool readLine(char expected, size_t length)
{
	uint8_t data;
	for( size_t index = 0U; index < length; index++ )
	{
		if( !pc_consoleReadByte(&data) || (data != (uint8_t)expected) ) { return false; }
	}
	return !pc_consoleReadByte(&data);
}

static bool checkBoundary(size_t typed, size_t accepted)
{
	int keys[TESTCONSOLE_LINE_CAPACITY + 1U];
	for( size_t index = 0U; index < typed; index++ ) { keys[index] = 'x'; }
	keys[typed] = '\n';
	if( !queueKeys(keys, typed + 1U) ) { return false; }
	int before = refresh_count;
	pc_consolePollInput();
	return (refresh_count == before + 1) && readLine('x', accepted);
}

static bool checkStage(bool condition, const char *stage)
{
	if( !condition )
	{
		fprintf(stderr, "input test failed at %s: refresh=%d cursor=%d\n",
				stage, refresh_count, getcurx(curscr));
	}
	return condition;
}

int main(int argc, char *argv[])
{
	if( !pc_consoleInit() ) { return EXIT_FAILURE; }
	refresh_count = 0U;

	if( (argc > 1) && (strcmp(argv[1U], "halt") == 0U) )
	{
		const int halt_key[] = {KEY_F(TESTCONSOLE_KEY_HALT)};
		if( !queueKeys(halt_key, 1U) ) { return EXIT_FAILURE; }
		pc_consolePollInput();
		return EXIT_FAILURE;
	}

	const int printable[] = {'a', 'b'};
	if( !queueKeys(printable, sizeof(printable) / sizeof(printable[0U])) )
	{
		return EXIT_FAILURE;
	}
	pc_consolePollInput();
	bool valid = checkStage((refresh_count == 1) &&
		screenHasText("ab", TESTCONSOLE_PRINTABLE_COUNT), "printable");

	const int deletions[] = {KEY_BACKSPACE, 'c', TESTCONSOLE_KEY_DELETE, 'd', '\b'};
	valid = valid && queueKeys(deletions, sizeof(deletions) / sizeof(deletions[0U]));
	int before = refresh_count;
	pc_consolePollInput();
	valid = checkStage((refresh_count == before + 1) && screenHasText("a", 1U),
		"deletions") && valid;

	const int enter[] = {'\n'};
	valid = valid && queueKeys(enter, 1U);
	before = refresh_count;
	pc_consolePollInput();
	valid = checkStage((refresh_count == before + 1) && readLine('a', 1U) &&
		screenHasText("", 0U), "enter") && valid;

	valid = checkStage(checkBoundary(0U, 0U), "empty") && valid;
	const int empty_deletions[] = {KEY_BACKSPACE, TESTCONSOLE_KEY_DELETE, '\b'};
	valid = valid && queueKeys(empty_deletions,
		sizeof(empty_deletions) / sizeof(empty_deletions[0U]));
	before = refresh_count;
	pc_consolePollInput();
	valid = checkStage(refresh_count == before, "empty deletions") && valid;
	const int keypad_enter[] = {KEY_ENTER};
	valid = valid && queueKeys(keypad_enter, 1U);
	before = refresh_count;
	pc_consolePollInput();
	valid = checkStage((refresh_count == before + 1) && readLine('x', 0U),
		"keypad enter") && valid;
	valid = checkStage(checkBoundary(TESTCONSOLE_LINE_SHORT, TESTCONSOLE_LINE_SHORT),
		"62 bytes") && valid;
	valid = checkStage(checkBoundary(TESTCONSOLE_LINE_LIMIT, TESTCONSOLE_LINE_LIMIT),
		"63 bytes") && valid;
	valid = checkStage(checkBoundary(TESTCONSOLE_LINE_CAPACITY, TESTCONSOLE_LINE_LIMIT),
		"64 bytes") && valid;

	const int pending[] = {'q', '\r', 'z'};
	valid = valid && queueKeys(pending, sizeof(pending) / sizeof(pending[0U]));
	before = refresh_count;
	pc_consolePollInput();
	valid = checkStage(refresh_count == before + 1, "pending first render") && valid;
	before = refresh_count;
	pc_consolePollInput();
	valid = checkStage(refresh_count == before, "pending line preserved") && valid;
	uint8_t data;
	valid = valid && pc_consoleReadByte(&data) && (data == 'q');
	before = refresh_count;
	pc_consolePollInput();
	valid = checkStage((refresh_count == before + 1) && screenHasText("z", 1U),
		"pending remainder") && valid;

	pc_consoleShutdown();
	return valid ? EXIT_SUCCESS : EXIT_FAILURE;
}
