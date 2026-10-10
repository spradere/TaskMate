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

#include "../fileUtility.h"
#include "../tokenizer.h"
#include "tagWriters.h"

enum { TAGWRITERSGPIO_COUNT_FIELDS = 1U };

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void tagWriterWriteGpioSignals(const tag_writer_context_t *context)
{
	file_t file_signals;
	fileInit(&file_signals);
	file_signals.name = (char *)context->auto_options->file_gpio_signals;
	if( fileOpen(&file_signals, "r", FILEUTILITY_MODE_READONLY) != 0U )
	{
		AUTOCODE_MSG_ERROR("opening file <%s>", file_signals.name);
		*context->file_error = true;
		return;
	}

	fprintf(context->file, "typedef enum __attribute__((packed))\n");
	fprintf(context->file, "{\n");

	tokenizer_t tok = {0U};
	int line = 0U;
	file_get_line_result_t line_result;
	// Emit the first token of each signal line and report any extra tokens.
	while( (line_result = fileGetLine(&file_signals, tok.line, sizeof(tok.line))) ==
		   FILE_GET_LINE_SUCCESS )
	{
		line++;
		tokenizer_err_t token_error = tokenizer(&tok);
		if( token_error != TOK_ERR_NOERR )
		{
			AUTOCODE_MSG_ERROR("tokenizer [%s:%i]: %s",
							   file_signals.name,
							   line,
							   tokenizerErrorMessage(token_error));
			continue;
		}
		if( (tok.count != 0U) && (tok.tokens[0U][0U] != '#') )
		{
			if( tok.count > TAGWRITERSGPIO_COUNT_FIELDS )
			{
				AUTOCODE_MSG_ERROR(
					"in file %s wrong token count line %i\n", file_signals.name, line);
			}
			fprintf(context->file, "\t%s,\n", tok.tokens[0U]);
		}
	}
	if( line_result == FILE_GET_LINE_ERROR )
	{
		AUTOCODE_MSG_ERROR("reading file <%s> after line %i", file_signals.name, line);
		*context->file_error = true;
	}
	// The final enum value sizes the logical signal table in the target.
	fprintf(context->file, "\tGPIO_SIGNAL_COUNT\n");
	fprintf(context->file, "} gpio_signal_t;\n");
	fprintf(context->file,
			"_Static_assert(sizeof(gpio_signal_t) == 1U, \"gpio_signal_t must be one byte\");\n");

	// Release parser storage and report any failure to close the signal source.
	tokenizerFree(&tok);
	if( fileClose(&file_signals) != FILE_UTILITY_OK )
	{
		AUTOCODE_MSG_ERROR("closing GPIO signals file");
		*context->file_error = true;
	}
}

void tagWriterWriteWireGpio(const tag_writer_context_t *context)
{
	const char *wire_gpio = context->auto_options->file_wire_gpio;

	fprintf(context->file, "#include \"%s\"\n", wire_gpio);
}
