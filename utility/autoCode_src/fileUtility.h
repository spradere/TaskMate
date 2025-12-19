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
 * @file fileUtility.h
 * @brief file utility header
 *
 */

 #ifndef FILE_H
 #define FILE_H

#include "utility/autoCode_src/autoCode.h"
#include <stdbool.h>

typedef struct
{
	FILE *stream;
	bool stream_open; // allow fclose()
	char *name;
	bool name_alloc; // free() ?
} file_t;

void fileInit(file_t *file);
void fileOpen( file_t *file, const char *mode, const char *caller);
void fileMakeTmp(const char *file_src_name, file_t *file_tmp, const char *caller);
void printLicenceHeader(FILE *file);
void printWarningHeader(FILE *file);

 #endif
