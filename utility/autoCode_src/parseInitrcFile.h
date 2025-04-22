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
 * @file parseinitrcFile.h
 * @brief read init.rc file header
 *
 * @todo nothing
 */

#ifndef LISTTOTABLE_H
#define LISTTOTABLE_H

typedef enum
{
	MODULE_TYPE_DRIVER,
	MODULE_TYPE_SERVICE,
	MODULE_TYPE_TASK
} modules_type_t;

void parseInitrcFile(module_t *modules, char *file_name, modules_type_t type);

#endif
