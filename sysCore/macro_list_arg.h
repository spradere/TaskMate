// example #define TASK_LIST (task1, task2, task3)

// count items in list -> N
#define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1,0)
#define COUNT_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10, N, ...) N 

//#define TASK_COUNT COUNT_ARGS(TASK_LIST) automated count -> task_table[TASK_COUNT];

// extract list items to taskCreate();
// Expand the list: calls APPLY_FUNC(task1), APPLY_FUNC(task2), ...
#define TASK_CREATE_EXPAND_LIST(...) FOR_EACH(TASK_CREATE_APPLY_FUNC, __VA_ARGS__)

// Macro to apply a function to each task
#define TASK_CREATE_APPLY_FUNC(task) taskcreate(#task,i++);

// Generic macro to apply FUNC to each argument
#define FOR_EACH(FUNC, ...) __VA_OPT__(FOR_EACH_IMPL(FUNC, __VA_ARGS__))

// Recursive helper macro (unrolls each argument)
#define FOR_EACH_IMPL(FUNC, first, ...) FUNC(first) __VA_OPT__(FOR_EACH_IMPL(FUNC, __VA_ARGS__))
