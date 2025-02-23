#include <stdio.h>

#define PRINT(what) printf("%s -> %2d \n", #what, what)


#define TASK_LIST task1, task2, a,b

// count items in list -> N
#define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1,0)
#define COUNT_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10, N, ...) N 

/* Macro to apply a function to each task
#define TASK_CREATE_APPLY_FUNC(task) taskCreate(task, i++);

// Recursive macro expansion fix: Expand in multiple steps
#define EXPAND(...) __VA_ARGS__

// Generic macro to apply FUNC to each argument
#define FOR_EACH(FUNC, ...) EXPAND(FOR_EACH_IMPL(FUNC, __VA_ARGS__))

// Recursive helper macro (unrolls each argument)
#define FOR_EACH_IMPL(FUNC, first, ...) \
    FUNC(first) __VA_OPT__(EXPAND(FOR_EACH_IMPL(FUNC, __VA_ARGS__)))

// Apply FOR_EACH to generate taskCreate() calls
#define TASK_CREATE_EXPAND_LIST(...) FOR_EACH(TASK_CREATE_APPLY_FUNC, TASK_LIST)*/

// Step 2: Define indexed extraction
#define GET_TASK_1 task1
#define GET_TASK_2 task2
#define GET_TASK_3 task3
#define GET_TASK_4 task4

#define GET_TASK(N) GET_TASK_##N  // Extract task using index

// Step 3: Expand initialization calls using index
#define EXPAND_TASKS \
    taskCreate(GET_TASK(1), 0); \
    taskCreate(GET_TASK(2), 1); \
    taskCreate(GET_TASK(3), 2); \
    taskCreate(GET_TASK(4), 3);


void test(void)
{
	int i=0;
	EXPAND_TASKS(TASK_LIST)
}


int main(void)
{
    PRINT(COUNT_ARGS(A));
    PRINT(COUNT_ARGS(A, B));
    PRINT(COUNT_ARGS(A, B, C));
    PRINT(COUNT_ARGS(A, B, C, D));
    PRINT(COUNT_ARGS(TASK_LIST));
    
	return 0;	
}
