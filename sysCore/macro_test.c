#include <stdio.h>

#define PRINT(what) printf("%s -> %2d \n", #what, what)


#define TASK_LIST task1, task2

// count items in list -> N
#define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1,0)
#define COUNT_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10, N, ...) N 

// Macro to apply a function to each task
#define TASK_CREATE_APPLY_FUNC(task) taskCreate(task, i++);

// Recursive macro expansion fix: Expand in multiple steps
#define EXPAND(...) __VA_ARGS__

// Generic macro to apply FUNC to each argument
#define FOR_EACH(FUNC, ...) EXPAND(FOR_EACH_IMPL(FUNC, __VA_ARGS__))

// Recursive helper macro (unrolls each argument)
#define FOR_EACH_IMPL(FUNC, first, ...) \
    FUNC(first) __VA_OPT__(EXPAND(FOR_EACH_IMPL(FUNC, __VA_ARGS__)))

// Apply FOR_EACH to generate taskCreate() calls
#define TASK_CREATE_EXPAND_LIST(...) FOR_EACH(TASK_CREATE_APPLY_FUNC, TASK_LIST)




void test(void)
{
	int i=0;
	TASK_CREATE_EXPAND_LIST(TASK_LIST)
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
