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

#define FILE_READONLY 1
#define FILE_CREATE 2

typedef struct
{
	FILE *stream;
	bool stream_opened; // allow fclose()
	char *name;
	bool name_allocated; // allow free()
	char token[FILE_TOKEN_SIZE];
} file_t;

void filePrintModified(void);
void fileCmpReplace(file_t *file_old, file_t *file_new);
void fileClose(file_t *file, const char *caller, int line);
void fileInit(file_t *file);
void fileOpen( file_t *file, const char *mode, int special_mode, const char *caller, int line);
void fileMakeTmp(const char *file_src_name, file_t *file_tmp, const char *caller, int line);
void printLicenceHeader(FILE *file);
void printWarningHeader(FILE *file);
void printClangFormatOff(FILE *file);
void printClangFormatOn(FILE *file);
void generateGuardName(const char *file_name, char *guard_name);

#endif
