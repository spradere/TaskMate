/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file pc_console.c
 * @brief PC ncurses console implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "pc_console.h"

#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "interfaces/hal_halt.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

// ncurses uses int for geometry and keys, including negative error values.
#define PCCONSOLE_MIN_COLUMNS 60
#define PCCONSOLE_MIN_ROWS 16
#define PCCONSOLE_HEIGHT_INPUT 3
#define PCCONSOLE_SIZE_INPUT 64
#define PCCONSOLE_WIDTH_BORDER 2
#define PCCONSOLE_COLUMN_LABEL 2
#define PCCONSOLE_WIDTH_HALT_LABEL 13
#define PCCONSOLE_KEY_HALT 10
#define PCCONSOLE_KEY_DELETE 0x7F
#define PCCONSOLE_CHAR_PRINTABLEMIN 0x20
#define PCCONSOLE_CHAR_PRINTABLEMAX 0x7E

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static WINDOW *output_frame;
static WINDOW *output_console;
static WINDOW *input_frame;
static WINDOW *input_console;
static bool console_started;
static FILE *console_stdout;
static FILE *terminal_stdout;

static uint8_t input_edit[PCCONSOLE_SIZE_INPUT];
static uint8_t input_ready[PCCONSOLE_SIZE_INPUT];
static uint8_t input_edit_length;
static uint8_t input_ready_length;
static uint8_t input_ready_index;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void consoleDeleteWindows(void);
static void consoleDrawFrames(void);
static void consoleInputRender(void);
static int consoleStdoutWrite(void *cookie, const char *buffer, int length);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static void consoleDeleteWindows(void)
{
	if( output_console != NULL ) { delwin(output_console); }
	if( input_console != NULL ) { delwin(input_console); }
	if( output_frame != NULL ) { delwin(output_frame); }
	if( input_frame != NULL ) { delwin(input_frame); }

	output_console = NULL;
	input_console = NULL;
	output_frame = NULL;
	input_frame = NULL;
}

static void consoleDrawFrames(void)
{
	box(output_frame, 0U, 0U);
	box(input_frame, 0U, 0U);
	mvwprintw(output_frame, 0U, PCCONSOLE_COLUMN_LABEL, " USART output ");
	mvwprintw(input_frame, 0U, PCCONSOLE_COLUMN_LABEL, " USART input ");
	mvwprintw(input_frame, 0U, getmaxx(input_frame) - PCCONSOLE_WIDTH_HALT_LABEL, " F10: halt ");
	wnoutrefresh(output_frame);
	wnoutrefresh(input_frame);
	wnoutrefresh(output_console);
	wnoutrefresh(input_console);
	doupdate();
}

/* Keep standard libc output inside the USART output window after ncurses captures the terminal. */
static int consoleStdoutWrite(void *cookie, const char *buffer, int length)
{
	(void)cookie;
	for( int index = 0U; index < length; index++ ) { pc_consoleWriteByte((uint8_t)buffer[index]); }
	pc_consoleFlush();
	return length;
}

/*
 * Set up the two terminal views and register shutdown after both windows are ready.
 * A terminal smaller than the fixed layout is rejected.
 */
bool pc_consoleInit(void)
{
	if( console_started ) { return true; }
	// Check terminal dimensions before allocating the two view windows.
	(void)setlocale(LC_ALL, "");
	if( initscr() == NULL ) { return false; }

	int rows;
	int columns;
	getmaxyx(stdscr, rows, columns);
	if( (rows < PCCONSOLE_MIN_ROWS) || (columns < PCCONSOLE_MIN_COLUMNS) )
	{
		endwin();
		return false;
	}

	// Allocate frames first, then the inner windows owned by those frames.
	const int output_width = columns;
	const int output_height = rows - PCCONSOLE_HEIGHT_INPUT;
	output_frame = newwin(output_height, output_width, 0U, 0U);
	input_frame = newwin(PCCONSOLE_HEIGHT_INPUT, output_width, output_height, 0U);
	if( (output_frame == NULL) || (input_frame == NULL) )
	{
		consoleDeleteWindows();
		endwin();
		return false;
	}

	output_console = derwin(output_frame,
							output_height - PCCONSOLE_WIDTH_BORDER,
							output_width - PCCONSOLE_WIDTH_BORDER,
							1U,
							1U);
	input_console = derwin(input_frame, 1U, output_width - PCCONSOLE_WIDTH_BORDER, 1U, 1U);
	if( (output_console == NULL) || (input_console == NULL) )
	{
		consoleDeleteWindows();
		endwin();
		return false;
	}

	// Configure terminal input before enabling nonblocking polling.
	if( (cbreak() == ERR) || (noecho() == ERR) )
	{
		consoleDeleteWindows();
		endwin();
		return false;
	}
	keypad(input_console, true);
	nodelay(input_console, true);
	scrollok(output_console, true);
	(void)curs_set(1U);

	// Register terminal cleanup only after initialization can be completed.
	input_edit_length = 0U;
	input_ready_length = 0U;
	input_ready_index = 0U;
	console_stdout = fwopen(NULL, consoleStdoutWrite);
	if( (console_stdout == NULL) || (setvbuf(console_stdout, NULL, _IONBF, 0U) != 0U) )
	{
		if( console_stdout != NULL ) { fclose(console_stdout); }
		consoleDeleteWindows();
		endwin();
		return false;
	}
	terminal_stdout = stdout;
	stdout = console_stdout;
	console_started = true;
	if( atexit(pc_consoleShutdown) != 0U )
	{
		pc_consoleShutdown();
		return false;
	}
	consoleDrawFrames();
	return true;
}

void pc_consoleShutdown(void)
{
	if( !console_started ) { return; }
	console_started = false;
	stdout = terminal_stdout;
	fclose(console_stdout);
	console_stdout = NULL;
	terminal_stdout = NULL;
	consoleDeleteWindows();
	endwin();
}

void pc_consoleWriteByte(uint8_t data)
{
	if( !console_started ) { return; }
	if( data == '\r' ) { return; }
	waddch(output_console, (chtype)data);
}

void pc_consoleFlush(void)
{
	if( console_started ) { wrefresh(output_console); }
}

static void consoleInputRender(void)
{
	werase(input_console);
	waddnstr(input_console, (const char *)input_edit, input_edit_length);
	wrefresh(input_console);
}

/*
 * Keep one completed input line until its bytes have been consumed. New key presses remain
 * in ncurses while that line is pending.
 */
void pc_consolePollInput(void)
{
	if( !console_started || (input_ready_index < input_ready_length) ) { return; }

	bool line_modified = false;
	int key;
	while( (key = wgetch(input_console)) != ERR )
	{
		if( key == KEY_F(PCCONSOLE_KEY_HALT) ) { hal_halt(); }
		// A submitted line becomes immutable until USART has read every byte.
		if( (key == '\n') || (key == '\r') || (key == KEY_ENTER) )
		{
			for( uint8_t i = 0U; i < input_edit_length; i++ ) { input_ready[i] = input_edit[i]; }
			input_ready_length = input_edit_length;
			input_ready_index = 0U;
			input_edit_length = 0U;
			consoleInputRender();
			return;
		}
		// Editing affects only the line still visible in the input window.
		if( (key == KEY_BACKSPACE) || (key == PCCONSOLE_KEY_DELETE) || (key == '\b') )
		{
			if( input_edit_length > 0U )
			{
				input_edit_length--;
				line_modified = true;
			}
			continue;
		}
		// Reserve one byte so the edit buffer never reaches its full capacity.
		if( (key >= PCCONSOLE_CHAR_PRINTABLEMIN) && (key <= PCCONSOLE_CHAR_PRINTABLEMAX) &&
			(input_edit_length < (PCCONSOLE_SIZE_INPUT - 1U)) )
		{
			input_edit[input_edit_length++] = (uint8_t)key;
			line_modified = true;
		}
	}
	if( line_modified ) { consoleInputRender(); }
}

bool pc_consoleReadByte(uint8_t *data)
{
	if( data == NULL ) { return false; }
	pc_consolePollInput();
	if( input_ready_index >= input_ready_length ) { return false; }

	*data = input_ready[input_ready_index++];
	if( input_ready_index == input_ready_length )
	{
		input_ready_index = 0U;
		input_ready_length = 0U;
	}
	return true;
}
