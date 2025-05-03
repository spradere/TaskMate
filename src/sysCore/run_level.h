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
 * @file run_level.h
 * @brief run level definition header
 *
 * @todo Nothing
 */

#ifndef RUN_LEVEL_H
#define RUN_LEVEL_H

// run levels definitions
#define RUN_NONE 0 /**< no start, could be started later via sCLI */
#define RUN_CORE 1 /**< start minimal critical system compoments, */
#define RUN_DRIVER 2 /**< start hardware drivers*/
#define RUN_SERVICE 3 /**< start system services*/
#define RUN_USER 4 /**< start user tasks*/

#define RUN_LEVEL_MASK 0xf8 /**< to save space run level is stored in status byte, the first 3 LSB bits are used*/
#define RUN_LEVEL_COUNT 5

typedef struct
{
	uint16_t list[];
	uint8_t count;

}run_level_item_t;


typedef struct
{
	run_level_item_t threads_list[RUN_LEVEL_COUNT];
	uint8_t current;
	uint8_t next;

} run_level_t;

/*
 * in TaskMate.c for scheduler
 * run_level_t run_level;
 *
 * run_level[0]={0};
 * run_level[1]={1000, 1001, 2002, 2010, 0};
 * run_level[2]={3003,0};
 * run_level[3]={2005,0};
 * run_level[4]={3004, 3005};
 *
 * run_level.current = RUN_CORE;
 * run_level.next = RUN_CORE;
 * */

#endif
