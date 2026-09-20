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
#include <stdlib.h>

#include "interfaces/hal_halt.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define PC_CONSOLE_COLUMN_COUNT_MIN 60
#define PC_CONSOLE_ROW_COUNT_MIN 16
#define PC_CONSOLE_GPIO_WIDTH 24
#define PC_CONSOLE_INPUT_HEIGHT 3
#define PC_CONSOLE_INPUT_SIZE 64u
#define PC_CONSOLE_COLOUR_OFF 1
#define PC_CONSOLE_COLOUR_ON 2

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static WINDOW *output_frame;
static WINDOW *output_console;
static WINDOW *input_frame;
static WINDOW *input_console;
static WINDOW *gpio_frame;
static WINDOW *gpio_console;
static bool console_started;
static bool console_colours;

static uint8_t input_edit[PC_CONSOLE_INPUT_SIZE];
static uint8_t input_ready[PC_CONSOLE_INPUT_SIZE];
static uint8_t input_edit_length;
static uint8_t input_ready_length;
static uint8_t input_ready_index;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void consoleDeleteWindows(void);
static void consoleDrawFrames(void);
static void consoleInputRender(void);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static void consoleDeleteWindows(void)
{
	if( output_console != 0 ) { delwin(output_console); }
	if( input_console != 0 ) { delwin(input_console); }
	if( gpio_console != 0 ) { delwin(gpio_console); }
	if( output_frame != 0 ) { delwin(output_frame); }
	if( input_frame != 0 ) { delwin(input_frame); }
	if( gpio_frame != 0 ) { delwin(gpio_frame); }

	output_console = 0;
	input_console = 0;
	gpio_console = 0;
	output_frame = 0;
	input_frame = 0;
	gpio_frame = 0;
}

static void consoleDrawFrames(void)
{
	box(output_frame, 0, 0);
	box(input_frame, 0, 0);
	box(gpio_frame, 0, 0);
	mvwprintw(output_frame, 0, 2, " USART output ");
	mvwprintw(input_frame, 0, 2, " USART input ");
	mvwprintw(gpio_frame, 0, 2, " GPIO LEDs ");
	mvwprintw(gpio_frame, getmaxy(gpio_frame) - 2, 2, " F10: halt ");
	wnoutrefresh(output_frame);
	wnoutrefresh(input_frame);
	wnoutrefresh(gpio_frame);
	wnoutrefresh(output_console);
	wnoutrefresh(input_console);
	wnoutrefresh(gpio_console);
	doupdate();
}

bool pc_consoleInit(void)
{
	if( console_started ) { return true; }
	(void)setlocale(LC_ALL, "");
	if( initscr() == 0 ) { return false; }

	int rows;
	int columns;
	getmaxyx(stdscr, rows, columns);
	if( (rows < PC_CONSOLE_ROW_COUNT_MIN) || (columns < PC_CONSOLE_COLUMN_COUNT_MIN) )
	{
		endwin();
		return false;
	}

	const int output_width = columns - PC_CONSOLE_GPIO_WIDTH;
	const int output_height = rows - PC_CONSOLE_INPUT_HEIGHT;
	output_frame = newwin(output_height, output_width, 0, 0);
	input_frame = newwin(PC_CONSOLE_INPUT_HEIGHT, output_width, output_height, 0);
	gpio_frame = newwin(rows, PC_CONSOLE_GPIO_WIDTH, 0, output_width);
	if( (output_frame == 0) || (input_frame == 0) || (gpio_frame == 0) )
	{
		consoleDeleteWindows();
		endwin();
		return false;
	}

	output_console = derwin(output_frame, output_height - 2, output_width - 2, 1, 1);
	input_console = derwin(input_frame, 1, output_width - 2, 1, 1);
	gpio_console = derwin(gpio_frame, rows - 2, PC_CONSOLE_GPIO_WIDTH - 2, 1, 1);
	if( (output_console == 0) || (input_console == 0) || (gpio_console == 0) )
	{
		consoleDeleteWindows();
		endwin();
		return false;
	}

	if( (cbreak() == ERR) || (noecho() == ERR) )
	{
		consoleDeleteWindows();
		endwin();
		return false;
	}
	keypad(input_console, true);
	nodelay(input_console, true);
	scrollok(output_console, true);
	(void)curs_set(1);

	if( has_colors() )
	{
		start_color();
		use_default_colors();
		init_pair(PC_CONSOLE_COLOUR_OFF, COLOR_WHITE, -1);
		init_pair(PC_CONSOLE_COLOUR_ON, COLOR_GREEN, -1);
		console_colours = true;
	}

	input_edit_length = 0;
	input_ready_length = 0;
	input_ready_index = 0;
	console_started = true;
	if( atexit(pc_consoleShutdown) != 0 )
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

void pc_consolePollInput(void)
{
	if( !console_started || (input_ready_index < input_ready_length) ) { return; }

	int key;
	while( (key = wgetch(input_console)) != ERR )
	{
		if( key == KEY_F(10) ) { hal_halt(); }
		if( (key == '\n') || (key == '\r') || (key == KEY_ENTER) )
		{
			for( uint8_t i = 0; i < input_edit_length; i++ )
			{
				input_ready[i] = input_edit[i];
			}
			input_ready_length = input_edit_length;
			input_ready_index = 0;
			input_edit_length = 0;
			consoleInputRender();
			return;
		}
		if( (key == KEY_BACKSPACE) || (key == 0x7f) || (key == '\b') )
		{
			if( input_edit_length > 0 ) { input_edit_length--; }
			consoleInputRender();
			continue;
		}
		if( (key >= 0x20) && (key <= 0x7e) &&
			(input_edit_length < (PC_CONSOLE_INPUT_SIZE - 1u)) )
		{
			input_edit[input_edit_length++] = (uint8_t)key;
			consoleInputRender();
		}
	}
}

bool pc_consoleReadByte(uint8_t *data)
{
	if( data == 0 ) { return false; }
	pc_consolePollInput();
	if( input_ready_index >= input_ready_length ) { return false; }

	*data = input_ready[input_ready_index++];
	if( input_ready_index == input_ready_length )
	{
		input_ready_index = 0;
		input_ready_length = 0;
	}
	return true;
}

void pc_consoleLedWrite(uint8_t index, const char *name, bool value)
{
	if( !console_started || (name == 0) ) { return; }
	pc_consolePollInput();

	if( console_colours )
	{
		wattron(gpio_console,
				COLOR_PAIR(value ? PC_CONSOLE_COLOUR_ON : PC_CONSOLE_COLOUR_OFF));
	}
	mvwprintw(gpio_console, index, 0, "%-12s [%s]", name, value ? "ON " : "OFF");
	if( console_colours )
	{
		wattroff(gpio_console,
				 COLOR_PAIR(value ? PC_CONSOLE_COLOUR_ON : PC_CONSOLE_COLOUR_OFF));
	}
	wrefresh(gpio_console);
}
