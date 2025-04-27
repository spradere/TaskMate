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
 * @file parseTag.h
 * @brief read tag header
 *
 * @todo nothing
 */

 #ifndef PARSETAG_H
 #define PARSETAG_H

void parseTag(module_t *modules, char *name_src);

void writeDriversInit(module_t *modules, FILE *file);
void writeThreadsInit(module_t *modules, FILE *file);


 #endif
