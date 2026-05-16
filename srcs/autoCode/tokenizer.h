/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tokenizer.h
 * @brief autoCode tokenizer header declarations.
 *
 */

#ifndef AUTOCODE_TOKENIZER_H
#define AUTOCODE_TOKENIZER_H

#include "autoCode.h"

// sizes for tokenizer
#define TOKEN_LINE_SIZE_MAX 256
#define TOKEN_COUNT_MAX 4
#define TOKEN_SIZE_MAX 64

typedef struct
{
	char line[TOKEN_LINE_SIZE_MAX];
	char tokens[TOKEN_COUNT_MAX][TOKEN_SIZE_MAX];
	int count;

}tokenizer_t;


void tokenizer(tokenizer_t *tok);

#endif // AUTOCODE_TOKENIZER_H
