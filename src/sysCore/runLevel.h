/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file runLevel.h
 * @brief run level header
 *
 * @todo Nothing
 */

#ifndef RUNLEVEL_H
#define RUNLEVEL_H

// run levels definitions
#define RUN_NONE 0 /**< no start, could be started later via sCLI */
#define RUN_CORE 1 /**< start minimal critical system compoments, */
#define RUN_DRIVER 2 /**< start hardware drivers*/
#define RUN_SERVICE 3 /**< start system services*/
#define RUN_USER 4 /**< start user tasks*/

#define RUN_LEVEL_MASK                                                                                       \
	0x07 /**< to save space run level is stored in status byte, the first 3 LSB bits are used*/
#define RUN_LEVEL_COUNT 5

void runLevelInit(void);

#endif
