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

/**
 * @brief Input line capacity in bytes for tokenization.
 */
#define TOKEN_LINE_SIZE_MAX 256

/**
 * @brief Holds a tokenized input line and its token pointers.
 */
typedef struct
{
	char line[TOKEN_LINE_SIZE_MAX];
	char **tokens;
	int count;

} tokenizer_t;

/**
 * @brief Reports tokenization results.
 */
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
 
/**
 * @brief Split a line into bounded tokens.
 * @param[in,out] tok Input line and resulting token pointers.
 * @return TOK_ERR_NOERR on success; a tokenizer error otherwise.
 */
tokenizer_err_t tokenizer(tokenizer_t *tok);
/**
 * @brief Get a diagnostic message for a tokenizer result.
 * @return Diagnostic text for the result.
 */
const char *tokenizerErrorMessage(tokenizer_err_t error);
/**
 * @brief Release token storage owned by a tokenizer.
 */
void tokenizerFree(tokenizer_t *tok);

#endif // AUTOCODE_TOKENIZER_H
