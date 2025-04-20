#ifndef AUTOALLOC_H
#define AOTOALLOC_H

#include "sysCore/module_items.h"

#define THREAD_COUNT 4
#define DRIVER_COUNT 4


typedef struct
{

	driver_item_t drivers[4];
	thread_item_t threads[4];
	uint8_t thread_current;

}modules_t;


#endif
