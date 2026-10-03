/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file fileUtility.h
 * @brief file utility header declarations.
 *
 */

#ifndef AUTOCODE_FILEUTILITY_H
#define AUTOCODE_FILEUTILITY_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include "autoCode.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

#define FILE_READONLY 1
#define FILE_MISSING_ALLOWED 2

typedef struct
{
	FILE *stream;
	bool stream_opened; // allow fclose()
	bool write_access; // report buffered write errors on close
	char *name;
	bool name_allocated; // allow free()
} file_t;

typedef enum
{
	FILE_UTILITY_OK = 0,
	FILE_GET_LINE_SUCCESS = FILE_UTILITY_OK,
	FILE_GET_LINE_EOF,
	FILE_UTILITY_DIFFERENT,
	FILE_GET_LINE_ERROR,
	FILE_UTILITY_INVALID,
	FILE_UTILITY_OPEN,
	FILE_UTILITY_WRITE,
	FILE_UTILITY_CLOSE,
	FILE_UTILITY_SEEK,
	FILE_UTILITY_REMOVE,
	FILE_UTILITY_RENAME,
	FILE_UTILITY_ALLOC,
	FILE_UTILITY_REGISTER
} file_utility_err_t;

typedef file_utility_err_t file_get_line_result_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

void filePrintModified(void);
file_utility_err_t fileCmpReplaceAll(void);
file_utility_err_t fileClose(file_t *file);
file_utility_err_t fileGetLine(file_t *file, char *line, size_t line_size_max);
void fileInit(file_t *file);
file_utility_err_t fileOpen(file_t *file, const char *mode, int special_mode);
file_utility_err_t fileMakeTmp(const char *file_src_name, file_t *file_tmp);
const char *fileUtilityErrorMessage(file_utility_err_t error);

#endif // AUTOCODE_FILEUTILITY_H
