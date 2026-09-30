/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tokenizer.c
 * @brief tokenizer implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "tokenizer.h"

/* -------------------------------------
 * Private API
 * -----------------------------------*/
 
static bool indexTestOverflow(int *index);
static bool incIndexTestOverflow(int *index);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static bool indexTestOverflow(int *index)
{
	bool overflow = true;
		
	if( (*index) < AC_BUFFER_SIZE ){overflow = false;}
	else{ AUTOCODE_MSG_ERROR("index overflow");}
		
	return overflow;
}

static bool incIndexTestOverflow(int *index)
{
	(*index)++;
	bool overflow = indexTestOverflow(index);
	
	if( overflow == true ){(*index)--;}
	
	return overflow;
}


int tokenizer(tokenizer_t *tok)
{
	int index=0;

	// Start reading line to extract tokens
	tokenizerFree(tok);

	while(	(tok->line[index] != '\n') && 
			(tok->line[index] != 0) && 
			(indexTestOverflow(&index) == false) )
	{
		// Skip leading spaces and tabs
		while( (tok->line[index] == ' ') || (tok->line[index] == '\t') ) { incIndexTestOverflow(&index); }

		if( (tok->line[index] == '\n') || (tok->line[index] == 0) ) { break; }

		// Point to one token stored directly in line
		char cut_character = ' ';
		char *token = &tok->line[index];
		bool quoted_string = false;

		if( tok->line[index] == '"' ) // switch to string mode for this token
		{
			cut_character = '"';
			quoted_string = true;
			incIndexTestOverflow(&index);
		}

		while( (tok->line[index] != cut_character) && (quoted_string || (tok->line[index] != '\t')) &&
			   (tok->line[index] != '\n') && (tok->line[index] != 0) )
		{
			incIndexTestOverflow(&index);
		}

		if( quoted_string && (tok->line[index] != '"') )
		{
			AUTOCODE_MSG_ERROR("unterminated string");

			tokenizerFree(tok);
			return 1;
		}

		if( tok->line[index] == '"' ) { incIndexTestOverflow(&index); }

		char **tokens = realloc(tok->tokens, (size_t)(tok->count + 1) * sizeof(*tok->tokens));
		if( tokens == NULL )
		{
			AUTOCODE_MSG_ERROR("realloc tokenizer token %i", tok->count);

			tokenizerFree(tok);
			return 1;
		}
		tok->tokens = tokens;
		tok->tokens[tok->count] = token;

		if( tok->line[index] != 0 )
		{
			tok->line[index] = 0;
			incIndexTestOverflow(&index);
		}
		tok->count++;
	}

	return 0;
}

void tokenizerFree(tokenizer_t *tok)
{
	free(tok->tokens);
	tok->tokens = NULL;
	tok->count = 0;
}
