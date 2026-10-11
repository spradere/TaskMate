/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_pc_console_stdout.c
 * @brief Check that host libc stdout reaches the ncurses output window.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "hal/board/pc/pc_console.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define TEST_CONSOLE_OUTPUT_ROW 1
#define TEST_CONSOLE_OUTPUT_COLUMN 1

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

int main(void)
{
	FILE *original_stdout = stdout;
	if( !pc_consoleInit() ) { return EXIT_FAILURE; }

	bool stream_replaced = stdout != original_stdout;
	int written = printf("libc output");
	bool output_visible = (mvwinch(curscr, TEST_CONSOLE_OUTPUT_ROW, TEST_CONSOLE_OUTPUT_COLUMN) &
		A_CHARTEXT) == 'l';

	pc_consoleShutdown();
	if( !stream_replaced || (written == EOF) || !output_visible || (stdout != original_stdout) )
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
