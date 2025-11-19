
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
 * @file error.c
 * @brief System wide error implementation
 *
 * @todo Timeout routine, critical/fatal errors handling
 */

#include "sysCore/TaskMate_private_extern.h"
#include "sysCore/error.h"

volatile errorCode_t system_last_critical_error = ERR_SUCCESS;

void setCriticalError(errorCode_t err)
{
	system_last_critical_error = err;

	// Take immediate action for critical errors
	/*switch (err)
	{
		case ERR_FATAL_STACK_OVERFLOW:
		case ERR_FATAL_MEMORY_CORRUPTION:
		case ERR_FATAL_HW_FAILURE:
			printf("[FATAL ERROR] System killing thread ...\n");
			threadKillCurrent();
			break;
		default:
			break;
	}*/
}
