/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file fileUtility.c
 * @brief file utility implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "fileUtility.h"

#include <limits.h>
#include <sys/stat.h>

/* -----------------------------------------------
 * Private types
 * ---------------------------------------------*/

typedef struct
{
	char *source_name;
	char *temporary_name;
	bool active;
} file_tmp_item_t;

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static int file_updated = 0;
static int file_unchanged = 0;
static file_tmp_item_t *file_tmp_list = NULL;
static size_t file_tmp_source_count = 0;
static bool file_tmp_cleanup_registered = false;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static file_utility_err_t fileCompare(file_t *file_old, file_t *file_new);
static void fileTmpCleanup(void);
static file_utility_err_t fileTmpCleanupAll(void);
static file_utility_err_t fileTmpRegister(const char *file_src_name, char **temporary_name);
static file_utility_err_t fileTmpName(const char *file_src_name, char **temporary_name);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void filePrintModified(void)
{
	AUTOCODE_MSG_INFO("*******************************************************");
	AUTOCODE_MSG_INFO(
		"* summary of modified files : %i updated, %i unchanged *", file_updated, file_unchanged);
	AUTOCODE_MSG_INFO("*******************************************************");
}

file_utility_err_t fileCmpReplaceAll(void)
{
	file_utility_err_t result = FILE_UTILITY_OK;

	for( size_t i = 0; i < file_tmp_source_count; i++ )
	{
		file_t file_src;
		fileInit(&file_src);
		file_src.name = file_tmp_list[i].source_name;
		result = fileOpen(&file_src, "r", FILE_MISSING_ALLOWED);
		if( result != FILE_UTILITY_OK )
		{
			goto exit;
		}

		file_t file_tmp;
		fileInit(&file_tmp);
		file_tmp.name = file_tmp_list[i].temporary_name;
		result = fileOpen(&file_tmp, "r", FILE_READONLY);
		if( result != FILE_UTILITY_OK )
		{
			(void)fileClose(&file_src);
			goto exit;
		}

		const file_utility_err_t comparison = fileCompare(&file_src, &file_tmp);
		if( (comparison != FILE_UTILITY_OK) && (comparison != FILE_UTILITY_DIFFERENT) )
		{
			result = comparison;
		}
		file_utility_err_t close_result = fileClose(&file_src);
		if( result == FILE_UTILITY_OK ) { result = close_result; }
		close_result = fileClose(&file_tmp);
		if( result == FILE_UTILITY_OK ) { result = close_result; }
		if( result != FILE_UTILITY_OK ) { goto exit; }

		if( comparison == FILE_UTILITY_OK )
		{
			AUTOCODE_MSG_INFO("keep the old one <%s>", file_tmp_list[i].source_name);
			if( remove(file_tmp_list[i].temporary_name) != 0 )
			{
				result = FILE_UTILITY_REMOVE;
				goto exit;
			}
			file_tmp_list[i].active = false;
			file_unchanged++;
		}
		else
		{
			AUTOCODE_MSG_INFO("change for the new one, tmp -> <%s>", file_tmp_list[i].source_name);
			if( rename(file_tmp_list[i].temporary_name, file_tmp_list[i].source_name) != 0 )
			{
				result = FILE_UTILITY_RENAME;
				goto exit;
			}
			file_tmp_list[i].active = false;
			file_updated++;
		}
	}

exit:
	{
		file_utility_err_t cleanup_result = fileTmpCleanupAll();
		if( result == FILE_UTILITY_OK ) { result = cleanup_result; }
	}
	return result;
}

static file_utility_err_t fileCompare(file_t *file_old, file_t *file_new)
{
	char old[AC_BUFFER_SIZE];
	char new[AC_BUFFER_SIZE];
	bool same = true;
	file_utility_err_t result = FILE_UTILITY_OK;

	if( (file_old->stream != NULL) && (fseek(file_old->stream, 0L, SEEK_SET) != 0) )
	{
		result = FILE_UTILITY_SEEK;
		goto exit;
	}

	if( fseek(file_new->stream, 0L, SEEK_SET) != 0 )
	{
		result = FILE_UTILITY_SEEK;
		goto exit;
	}
	if( file_old->stream == NULL )
	{
		result = FILE_UTILITY_DIFFERENT;
		goto exit;
	}

	while( true )
	{
		file_utility_err_t old_result = fileGetLine(file_old, old, sizeof(old));
		file_utility_err_t new_result = fileGetLine(file_new, new, sizeof(new));

		if( (old_result == FILE_GET_LINE_ERROR) || (new_result == FILE_GET_LINE_ERROR) )
		{
			result = FILE_GET_LINE_ERROR;
			goto exit;
		}
		if( (old_result == FILE_GET_LINE_EOF) || (new_result == FILE_GET_LINE_EOF) )
		{
			same = (old_result == new_result);
			break;
		}
		if( strcmp(old, new) != 0 )
		{
			same = false;
			break;
		}
	}

	if( same == false ) { result = FILE_UTILITY_DIFFERENT; }
exit:
	return result;
}

file_utility_err_t fileGetLine(file_t *file, char *line, const size_t line_size_max)
{
	file_utility_err_t result = FILE_GET_LINE_SUCCESS;
	if( (file == NULL) || (file->stream == NULL) || (line == NULL) ||
		(line_size_max < 2U) || (line_size_max > (size_t)INT_MAX) )
	{
		result = FILE_GET_LINE_ERROR;
		goto exit;
	}

	if( fgets(line, (int)line_size_max, file->stream) == NULL )
	{
		result = feof(file->stream) ? FILE_GET_LINE_EOF : FILE_GET_LINE_ERROR;
		goto exit;
	}

	const size_t line_length = strlen(line);
	if( line_length == 0U )
	{
		result = FILE_GET_LINE_ERROR;
		goto exit;
	}

	if( line_length >= line_size_max - 1 )
	{
		result = FILE_GET_LINE_ERROR;
		goto exit;
	}

	if( line[line_length - 1U] == '\n' ) { goto exit; }

	/* Distinguish a valid final line that exactly fills the buffer from a truncated line. */
	if( feof(file->stream) ) { goto exit; }

	const int next_character = fgetc(file->stream);
	if( (next_character == EOF) && feof(file->stream) ) { goto exit; }
	result = FILE_GET_LINE_ERROR;

exit:
	return result;
}

file_utility_err_t fileClose(file_t *file)
{
	file_utility_err_t result = FILE_UTILITY_OK;
	if( file == NULL )
	{
		result = FILE_UTILITY_INVALID;
		goto exit;
	}

	if( file->stream_opened )
	{
		if( file->write_access && (ferror(file->stream) != 0) )
		{
			result = FILE_UTILITY_WRITE;
		}
		int err = fclose(file->stream);
		if( err != 0 )
		{
			result = FILE_UTILITY_CLOSE;
		}
		if( file->name_allocated ) { free(file->name); }
		fileInit(file);
	}
exit:
	return result;
}

void fileInit(file_t *file)
{
	file->name = NULL;
	file->name_allocated = false;
	file->stream = NULL;
	file->stream_opened = false;
	file->write_access = false;
}

file_utility_err_t fileOpen(file_t *file, const char *mode, const int special_mode)
{
	file_utility_err_t result = FILE_UTILITY_OK;
	if( (file == NULL) || (mode == NULL) || (file->name == NULL) )
	{
		result = FILE_UTILITY_INVALID;
		goto exit;
	}

	file->stream = fopen(file->name, mode);
	if( file->stream == NULL )
	{
		struct stat file_info;
		if( (special_mode != FILE_MISSING_ALLOWED) || (strcmp(mode, "r") != 0) ||
			(stat(file->name, &file_info) == 0) )
		{
			result = FILE_UTILITY_OPEN;
		}
		goto exit;
	}
	file->stream_opened = true;
	file->write_access =
		(strchr(mode, 'w') != NULL) || (strchr(mode, 'a') != NULL) || (strchr(mode, '+') != NULL);
exit:
	return result;
}

file_utility_err_t fileMakeTmp(const char *file_src_name, file_t *file_tmp)
{
	file_utility_err_t result = FILE_UTILITY_OK;
	if( (file_src_name == NULL) || (file_tmp == NULL) )
	{
		result = FILE_UTILITY_INVALID;
		goto exit;
	}
	result = fileTmpRegister(file_src_name, &file_tmp->name);
	if( result != FILE_UTILITY_OK ) { goto exit; }

	file_tmp->stream = fopen(file_tmp->name, "w+");
	if( file_tmp->stream == NULL )
	{
		result = FILE_UTILITY_OPEN;
		goto exit;
	}
	file_tmp->stream_opened = true;
	file_tmp->write_access = true;
	file_tmp_list[file_tmp_source_count - 1U].active = true;
exit:
	return result;
}

static void fileTmpCleanup(void) { (void)fileTmpCleanupAll(); }

static file_utility_err_t fileTmpCleanupAll(void)
{
	file_utility_err_t result = FILE_UTILITY_OK;

	for( size_t i = 0; i < file_tmp_source_count; i++ )
	{
		if( file_tmp_list[i].active && (remove(file_tmp_list[i].temporary_name) != 0) )
		{
			result = FILE_UTILITY_REMOVE;
		}
		free(file_tmp_list[i].temporary_name);
		free(file_tmp_list[i].source_name);
	}

	free(file_tmp_list);
	file_tmp_list = NULL;
	file_tmp_source_count = 0;
	return result;
}

static file_utility_err_t fileTmpRegister(const char *file_src_name, char **temporary_name)
{
	file_utility_err_t result = FILE_UTILITY_OK;
	char *source_name = NULL;
	*temporary_name = NULL;
	if( file_tmp_cleanup_registered == false )
	{
		if( atexit(fileTmpCleanup) != 0 )
		{
			result = FILE_UTILITY_REGISTER;
			goto exit;
		}
		file_tmp_cleanup_registered = true;
	}

	const size_t source_name_size = strlen(file_src_name) + 1;
	source_name = malloc(source_name_size);
	if( source_name == NULL )
	{
		result = FILE_UTILITY_ALLOC;
		goto exit;
	}
	memcpy(source_name, file_src_name, source_name_size);
	result = fileTmpName(file_src_name, temporary_name);
	if( result != FILE_UTILITY_OK )
	{
		goto exit;
	}

	file_tmp_item_t *list = realloc(file_tmp_list, (file_tmp_source_count + 1) * sizeof(*list));
	if( list == NULL )
	{
		result = FILE_UTILITY_ALLOC;
		goto exit;
	}

	file_tmp_list = list;
	file_tmp_list[file_tmp_source_count].source_name = source_name;
	file_tmp_list[file_tmp_source_count].temporary_name = *temporary_name;
	file_tmp_list[file_tmp_source_count].active = false;
	file_tmp_source_count++;
exit:
	if( result != FILE_UTILITY_OK )
	{
		free(*temporary_name);
		*temporary_name = NULL;
		free(source_name);
	}
	return result;
}

static file_utility_err_t fileTmpName(const char *file_src_name, char **temporary_name)
{
	file_utility_err_t result = FILE_UTILITY_OK;
	const size_t name_size = strlen(file_src_name) + sizeof(".tmp");
	*temporary_name = malloc(name_size);
	if( *temporary_name == NULL )
	{
		result = FILE_UTILITY_ALLOC;
		goto exit;
	}
	snprintf(*temporary_name, name_size, "%s.tmp", file_src_name);
exit:
	return result;
}

const char *fileUtilityErrorMessage(file_utility_err_t error)
{
	const char *message = "unknown file utility error";
	switch( error )
	{
		case FILE_UTILITY_OK: message = "no error"; break;
		case FILE_GET_LINE_EOF: message = "end of file"; break;
		case FILE_UTILITY_DIFFERENT: message = "files differ"; break;
		case FILE_GET_LINE_ERROR: message = "reading line failed or line too long"; break;
		case FILE_UTILITY_INVALID: message = "invalid file argument"; break;
		case FILE_UTILITY_OPEN: message = "opening file failed"; break;
		case FILE_UTILITY_WRITE: message = "writing file failed"; break;
		case FILE_UTILITY_CLOSE: message = "closing file failed"; break;
		case FILE_UTILITY_SEEK: message = "seeking file failed"; break;
		case FILE_UTILITY_REMOVE: message = "removing temporary file failed"; break;
		case FILE_UTILITY_RENAME: message = "renaming temporary file failed"; break;
		case FILE_UTILITY_ALLOC: message = "allocating file data failed"; break;
		case FILE_UTILITY_REGISTER: message = "registering temporary cleanup failed"; break;
	}
	goto exit;
exit:
	return message;
}
