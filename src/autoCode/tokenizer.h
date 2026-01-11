/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file tokenizer.h
 * @brief tokenizer header
 *
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

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

#endif
