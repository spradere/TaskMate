/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file scli_date.c
 * @brief Date command implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "scli_date.h"

#include "interfaces/drv_rtc.h"
#include "interfaces/tm_define.h"
#include "system/sysCall/sc_driver.h"
#include "system/sysCall/sc_errors.h"
#include "tmLibc/tm_string.h"
#include "tmLibc/tm_syslog.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define SCLIDATE_COUNT_SHOWARGS 1U
#define SCLIDATE_COUNT_SUBCOMMANDARGS 2U
#define SCLIDATE_COUNT_SETARGS 3U
#define SCLIDATE_INDEX_VALUE 2U
#define SCLIDATE_BASE_DECIMAL 10U
#define SCLIDATE_COUNT_TIMEDIGITS 2U
#define SCLIDATE_COUNT_YEARDIGITS 4U
#define SCLIDATE_TEXT_SIZE_MAX 11U
#define SCLIDATE_MAX_HOURS 23U
#define SCLIDATE_MAX_MINUTES 59U
#define SCLIDATE_MAX_SECONDS 59U
#define SCLIDATE_MAX_MONTH 12U
#define SCLIDATE_YEAR_FIRST 2000U
#define SCLIDATE_YEAR_LAST 2099U

/* -----------------------------------------------
 * Private types
 * ---------------------------------------------*/

typedef bool (*date_cmd_func_t)(uint8_t argc, char *argv[]);

typedef struct
{
	const char *name;
	date_cmd_func_t func;
} date_cmd_t;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static bool dateShow(uint8_t argc, char *argv[]);
static bool dateShowStartup(uint8_t argc, char *argv[]);
static bool dateSetTime(uint8_t argc, char *argv[]);
static bool dateSetDate(uint8_t argc, char *argv[]);
static bool dateHelp(uint8_t argc, char *argv[]);
static bool dateRead(hal_rtc_time_t *time);
static bool dateWrite(const hal_rtc_time_t *time);
static void datePrint(const hal_rtc_time_t *time);
static void datePrintError(err_codes_t error);
static bool dateParseField(const char *text, uint8_t *index, char separator,
						   uint8_t digit_count_min, uint8_t digit_count_max, uint16_t *value);
static bool dateParseTime(const char *text, hal_rtc_time_t *time);
static bool dateParseDate(const char *text, hal_rtc_time_t *time);

/* -----------------------------------------------
 * Command table
 * ---------------------------------------------*/

static const date_cmd_t date_cmd[] = {
	{"startup", dateShowStartup},
	{"time", dateSetTime},
	{"date", dateSetDate},
	{"help", dateHelp},
	{NULL, NULL},
};

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Command dispatch
 * ---------------------------------------------*/

bool dateCommand(uint8_t argc, char *argv[])
{
	if( argc == SCLIDATE_COUNT_SHOWARGS ) { return dateShow(argc, argv); }

	for( uint8_t i = 0U; date_cmd[i].name != NULL; i++ )
	{
		if( tm_strncmp(TM_STR_RAM(argv[1U]), TM_STR_RAM(date_cmd[i].name), TM_STRING_SIZE_MAX) ==
			0U )
		{
			return date_cmd[i].func(argc, argv);
		}
	}

	dateHelp(0U, NULL);
	return false;
}

/* -----------------------------------------------
 * Command handlers
 * ---------------------------------------------*/

static bool dateShow(uint8_t argc, char *argv[])
{
	(void)argc;
	(void)argv;

	hal_rtc_time_t time;
	if( !dateRead(&time) ) { return false; }
	datePrint(&time);
	return true;
}

static bool dateShowStartup(uint8_t argc, char *argv[])
{
	(void)argv;

	if( argc != SCLIDATE_COUNT_SUBCOMMANDARGS )
	{
		dateHelp(0U, NULL);
		return false;
	}

	hal_rtc_time_t time;
	err_codes_t error = sc_rtcGetStartupTime(&time);
	if( error != ERR_NO_ERROR )
	{
		datePrintError(error);
		return false;
	}

	datePrint(&time);
	return true;
}

static bool dateSetTime(uint8_t argc, char *argv[])
{
	if( argc != SCLIDATE_COUNT_SETARGS )
	{
		dateHelp(0U, NULL);
		return false;
	}

	hal_rtc_time_t time;
	if( !dateRead(&time) ) { return false; }
	if( !dateParseTime(argv[SCLIDATE_INDEX_VALUE], &time) )
	{
		tm_syslog(TM_STR("[date] invalid time, expected hh:mm:ss\n"));
		return false;
	}
	if( !dateWrite(&time) ) { return false; }

	datePrint(&time);
	return true;
}

static bool dateSetDate(uint8_t argc, char *argv[])
{
	if( argc != SCLIDATE_COUNT_SETARGS )
	{
		dateHelp(0U, NULL);
		return false;
	}

	hal_rtc_time_t time;
	if( !dateRead(&time) ) { return false; }
	if( !dateParseDate(argv[SCLIDATE_INDEX_VALUE], &time) )
	{
		tm_syslog(TM_STR("[date] invalid date, expected day/month/year (2000-2099)\n"));
		return false;
	}
	if( !dateWrite(&time) ) { return false; }

	datePrint(&time);
	return true;
}

static bool dateHelp(uint8_t argc, char *argv[])
{
	(void)argc;
	(void)argv;

	tm_syslog(TM_STR("[date] usage:\n"));
	tm_syslog(TM_STR("\tdate\n"));
	tm_syslog(TM_STR("\tdate startup\n"));
	tm_syslog(TM_STR("\tdate time hh:mm:ss\n"));
	tm_syslog(TM_STR("\tdate date day/month/year\n"));
	tm_syslog(TM_STR("\tdate help\n"));
	return true;
}

/* -----------------------------------------------
 * RTC access and reporting
 * ---------------------------------------------*/

static bool dateRead(hal_rtc_time_t *time)
{
	err_codes_t error = sc_rtcRead(time);
	if( error == ERR_NO_ERROR ) { return true; }
	datePrintError(error);
	return false;
}

static bool dateWrite(const hal_rtc_time_t *time)
{
	err_codes_t error = sc_rtcWrite(time);
	if( error == ERR_NO_ERROR ) { return true; }
	datePrintError(error);
	return false;
}

static void datePrint(const hal_rtc_time_t *time)
{
	tm_syslog(TM_STR("[date] %02i/%02i/20%02i %02i:%02i:%02i\n"),
			  time->day,
			  time->month,
			  time->year,
			  time->hours,
			  time->minutes,
			  time->seconds);
}

static void datePrintError(err_codes_t error)
{
	const tm_string_t *message = err_getMessage((uint8_t)error);
	if( message != NULL ) { tm_syslog(TM_STR("[date] RTC error: %s\n"), message); }
	else { tm_syslog(TM_STR("[date] RTC error\n")); }
}

/* -----------------------------------------------
 * Input parsing
 * ---------------------------------------------*/

static bool dateParseField(const char *text, uint8_t *index, char separator,
						   uint8_t digit_count_min, uint8_t digit_count_max, uint16_t *value)
{
	if( (text == NULL) || (index == NULL) || (value == NULL) ) { return false; }

	uint16_t parsed = 0U;
	uint8_t digit_count = 0U;
	while( (*index < SCLIDATE_TEXT_SIZE_MAX) && (digit_count < digit_count_max) &&
		(text[*index] >= '0') && (text[*index] <= '9') )
	{
		parsed = (parsed * SCLIDATE_BASE_DECIMAL) + (uint8_t)(text[*index] - '0');
		(*index)++;
		digit_count++;
	}
	if( (digit_count < digit_count_min) || (*index >= SCLIDATE_TEXT_SIZE_MAX) ) { return false; }

	if( separator == 0U )
	{
		if( text[*index] != 0U ) { return false; }
	}
	else
	{
		if( text[*index] != separator ) { return false; }
		(*index)++;
	}

	*value = parsed;
	return true;
}

static bool dateParseTime(const char *text, hal_rtc_time_t *time)
{
	if( (text == NULL) || (time == NULL) ) { return false; }

	uint8_t index = 0U;
	uint16_t hours;
	uint16_t minutes;
	uint16_t seconds;
	if( !dateParseField(text, &index, ':', SCLIDATE_COUNT_TIMEDIGITS, SCLIDATE_COUNT_TIMEDIGITS,
						&hours) ||
		!dateParseField(text, &index, ':', SCLIDATE_COUNT_TIMEDIGITS, SCLIDATE_COUNT_TIMEDIGITS,
						&minutes) ||
		!dateParseField(text, &index, 0U, SCLIDATE_COUNT_TIMEDIGITS, SCLIDATE_COUNT_TIMEDIGITS,
						&seconds) )
	{
		return false;
	}
	if( (hours > SCLIDATE_MAX_HOURS) || (minutes > SCLIDATE_MAX_MINUTES) ||
		(seconds > SCLIDATE_MAX_SECONDS) ) { return false; }

	time->hours = (uint8_t)hours;
	time->minutes = (uint8_t)minutes;
	time->seconds = (uint8_t)seconds;
	return true;
}

static bool dateParseDate(const char *text, hal_rtc_time_t *time)
{
	if( (text == NULL) || (time == NULL) ) { return false; }

	uint8_t index = 0U;
	uint16_t day;
	uint16_t month;
	uint16_t year;
	if( !dateParseField(text, &index, '/', 1U, SCLIDATE_COUNT_TIMEDIGITS, &day) ||
		!dateParseField(text, &index, '/', 1U, SCLIDATE_COUNT_TIMEDIGITS, &month) ||
		!dateParseField(text, &index, 0U, SCLIDATE_COUNT_YEARDIGITS, SCLIDATE_COUNT_YEARDIGITS,
						&year) )
	{
		return false;
	}
	if( (year < SCLIDATE_YEAR_FIRST) || (year > SCLIDATE_YEAR_LAST) || (month < 1U) ||
		(month > SCLIDATE_MAX_MONTH) ) { return false; }

	uint8_t rtc_year = (uint8_t)(year - SCLIDATE_YEAR_FIRST);

	time->day = (uint8_t)day;
	time->month = (uint8_t)month;
	time->year = rtc_year;
	return true;
}
