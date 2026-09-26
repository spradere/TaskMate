/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tagWriters_gpio.c
 * @brief autoCode GPIO tag writer implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "tagWriters.h"

#include "../fileUtility.h"
#include "../tokenizer.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void tagWriterWriteGpioSignals(const tag_writer_context_t *context)
{
	file_t file_signals;
	fileInit(&file_signals);
	file_signals.name = (char *)context->auto_options->file_gpio_signals;
	if( fileOpen(&file_signals, "r", FILE_READONLY, __FILE__, __LINE__) != 0 )
	{
		*context->file_error = true;
		return;
	}

	fprintf(context->file, "typedef enum\n");
	fprintf(context->file, "{\n");

	tokenizer_t tok = {0};
	int line = 0;
	file_get_line_result_t line_result;
	while( (line_result = fileGetLine(&file_signals, tok.line, sizeof(tok.line))) ==
		   FILE_GET_LINE_SUCCESS )
	{
		if( tokenizer(&tok) != 0 ) { continue; }
		line++;
		if( (tok.count != 0) && (tok.tokens[0][0] != '#') )
		{
			if( tok.count > 1 )
			{
				AUTOCODE_MSG_ERROR(
					"in file %s wrong token count line %i\n", file_signals.name, line);
			}
			fprintf(context->file, "\t%s,\n", tok.tokens[0]);
		}
	}
	if( line_result == FILE_GET_LINE_ERROR )
	{
		AUTOCODE_MSG_ERROR("reading file <%s> after line %i", file_signals.name, line);
		*context->file_error = true;
	}
	fprintf(context->file, "\tGPIO_SIGNAL_COUNT\n");
	fprintf(context->file, "} gpio_signal_t;\n");

	tokenizerFree(&tok);
	if( fileClose(&file_signals, __FILE__, __LINE__) != 0 ) { *context->file_error = true; }
}

void tagWriterWriteWireGpio(const tag_writer_context_t *context)
{
	const char *wire_gpio = context->auto_options->file_wire_gpio;

	fprintf(context->file, "#include \"%s\"\n", wire_gpio);
}
