/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tokenizer.h
 * @brief tokenizer header declarations.
 *
 */

#ifndef AUTOCODE_TOKENIZER_H
#define AUTOCODE_TOKENIZER_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include "autoCode.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

#define TOKEN_LINE_SIZE_MAX 256

typedef struct
{
	char line[TOKEN_LINE_SIZE_MAX];
	char **tokens;
	int count;

} tokenizer_t;

typedef enum
{
	TOK_ERR_NOERR,
	TOK_ERR_OF,
	TOK_ERR_STRING,
	TOK_ERR_ALLOC,
} tokenizer_err_t;

/* ============================================================================
 * Public API
 * ========================================================================== */
 
tokenizer_err_t tokenizer(tokenizer_t *tok);
const char *tokenizerErrorMessage(tokenizer_err_t error);
void tokenizerFree(tokenizer_t *tok);

#endif // AUTOCODE_TOKENIZER_H
