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

#include <assert.h>

/* -------------------------------------
 * Private API
 * -----------------------------------*/
 
static bool indexTestOverflow(size_t index);
static bool incIndexTestOverflow(int *index);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static bool indexTestOverflow(size_t index)
{
	bool overflow = false;
	if( index > (TOKENIZER_SIZE_LINEMAX - 1U) ){overflow = true;}
		
	return overflow;
}

static bool incIndexTestOverflow(int *index)
{
	(*index)++;
	bool overflow = indexTestOverflow((size_t)(*index));
	if( overflow == true ){(*index) = (TOKENIZER_SIZE_LINEMAX - 1U);}
	
	return overflow;
}

/*
 * Tokens point into tok->line. Separators are replaced with NUL bytes, so the input line
 * is modified and token pointers remain valid only while that line is unchanged.
 */
tokenizer_err_t tokenizer(tokenizer_t *tok)
{
	int index=0U;
	tokenizer_err_t tokenizer_status = TOK_ERR_NOERR;
	
	// Start reading the line to extract tokens
	tokenizerFree(tok);

	while(	(tok->line[index] != '\n') && 
			(tok->line[index] != 0U) )
	{
		// Skip leading spaces and tabs
		while( (tok->line[index] == ' ') || (tok->line[index] == '\t') ) 
		{ 
			bool status = incIndexTestOverflow(&index); 
			if(status == true){tokenizer_status = TOK_ERR_OF; goto exit;}
		}

		if( (tok->line[index] == '\n') || (tok->line[index] == 0U) ) { break; }

		// Point to one token stored directly in line
		char cut_character = ' ';
		char *one_token = &tok->line[index];
		bool quoted_string = false;

		if( tok->line[index] == '"' ) // switch to string mode for this token
		{
			cut_character = '"';
			quoted_string = true;
			bool status = incIndexTestOverflow(&index); 
			if(status == true){tokenizer_status = TOK_ERR_OF; goto exit;}			
		}

		// read charters
		while( 	(tok->line[index] != cut_character) && 
				(quoted_string || (tok->line[index] != '\t')) &&
				(tok->line[index] != '\n') && (tok->line[index] != 0U) )
		{
			bool status = incIndexTestOverflow(&index); 
			if(status == true){tokenizer_status = TOK_ERR_OF; goto exit;}
		}

		// quoted token handle
		if( quoted_string && (tok->line[index] != '"') )
		{
			tokenizer_status = TOK_ERR_STRING;
			goto exit;
		}

		if( tok->line[index] == '"' ) 
		{ 
			bool status = incIndexTestOverflow(&index); 
			if(status == true){tokenizer_status = TOK_ERR_OF; goto exit;}
		}

		// realloc tokens table
		char **tokens = realloc(tok->tokens, ((size_t)tok->count + 1U) * sizeof(*tok->tokens));
		if( tokens == NULL )
		{
			tokenizer_status = TOK_ERR_ALLOC;
			goto exit;	
		}
		tok->tokens = tokens;
		tok->tokens[tok->count] = one_token;

		// close current token
		tok->line[index] = 0U;
		bool status = incIndexTestOverflow(&index); 
		if(status == true){tokenizer_status = TOK_ERR_OF; goto exit;}

		tok->count++;
	}
	tokenizer_status = TOK_ERR_NOERR;

exit:
	if(tokenizer_status != TOK_ERR_NOERR){tokenizerFree(tok);}
	return tokenizer_status;
}

const char *tokenizerErrorMessage(tokenizer_err_t error)
{
	switch( error )
	{
		case TOK_ERR_OF: return "token line overflow";
		case TOK_ERR_STRING: return "unterminated string";
		case TOK_ERR_ALLOC: return "token allocation failed";
		case TOK_ERR_NOERR: return "no error";
	}
	return "unknown tokenizer error";
}

void tokenizerFree(tokenizer_t *tok)
{
	assert(tok != NULL);
	
	free(tok->tokens);
	tok->tokens = NULL;
	tok->count = 0U;
}
