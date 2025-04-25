#include "sysCore/modules_items.h"

#define DRIVERS_COUNT 4
#define THREADS_COUNT 4

typedef struct
{
	driver_item_t drivers[DRIVERS_COUNT];
	thread_item_t threads[THREADS_COUNT];
	uint8_t thread_current;
}modules_t;
