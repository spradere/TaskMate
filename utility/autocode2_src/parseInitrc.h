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
 * @file parseInitrc.h
 * @brief read init.rc file header
 *
 * @todo nothing
 */

#ifndef PARSEINITRC_H
#define PARSEINITRC_H

// type of file to parce
typedef enum {INITRC_DRIVERS, INITRC_SERVICES, INITRC_TASK} parse_init_rc_type_t;

void parseInitrc(parse_init_rc_type_t TYPE, module_base_t *modules, const char *file_name);

#endif
