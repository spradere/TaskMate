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

 #ifndef FILEUTILITY_H
 #define FILEUTILITY_H

#include "autoCode.h"

#define FILE_TOKEN_SIZE 256

typedef struct
{
	FILE *stream;
	bool stream_open; // allow fclose()
	char *name;
	bool name_alloc; // free() ?
	char token[FILE_TOKEN_SIZE];
} file_t;

int fileGetToken(file_t *file);
void filePrintTouch(void);
void fileCmpReplace(file_t *file_old, file_t *file_new);
void fileClose(file_t *file, const char *caller, const int line);
void fileInit(file_t *file);
void fileOpen( file_t *file, const char *mode, const char *caller, const int line);
void fileMakeTmp(const char *file_src_name, file_t *file_tmp, const char *caller, const int line);
void printLicenceHeader(FILE *file);
void printWarningHeader(FILE *file);

 #endif
