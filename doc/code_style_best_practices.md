# Code Style, Readability, Refactoring & Best Practices Summary

---

## The Golden Rule of Code

**Define once. Define clearly. Define where it makes sense.**
Promotes clarity, avoids duplication, and keeps logic in the right layer.

**Example:**
```c
#define DRIVER_STATUS_DEFAULT
modules->drivers[x]->status = DRIVER_STATUS_DEFAULT;
```

---

## Readability: Clarity over cleverness

Code is read more than it is written. Prioritize understandable variable names and structure.

**Example:**
```c
typeof(table->task_list[x]->status); // unclear
status_t taskStatus;                 // clear
```

---

## Maintainability: Avoid cryptic names and magic numbers

Use meaningful names, constants, and enums instead of hardcoded values.

**Example:**
```c
fs = 10;                         // bad
file_status = STATUS_DEFAULT;   // good
```

---

## Scalability: Replace long if/else chains with data structures

Use arrays, tables, or function pointers to reduce control flow complexity.

**Example:**
```c
command_table[] = {
    {"run", runFunc},
    {"stop", stopFunc}
};
```

---

## Self-documenting code over excessive comments

Write code so clear it needs fewer comments. Use comments to explain **why**, not **what**.

**Example:**
```c
// bad
int index = 10; // set index to 10

// good
int index = 10; // ignore first 10 entries in the list
```

---

## Three comment maturity levels

1. Describe what the code does (beginner)
2. Explain the logic’s purpose (intermediate)
3. Refactor code so comments are often unnecessary (advanced)

---

## Naming: Use plural for collections, singular for items

Helps indicate intent and structure of data.

**Example:**
```c
module_t *modules;  // collection
module_t module;    // single item
list_table_t *list_table; // unclear
```

---

## Abbreviations: Use only when standard or obvious

Avoid cryptic short forms unless they are universally recognized (e.g., UART, ISR).

**Example:**
```c
MSGERR();          // unclear
messageError();    // clear
```

---

## Error messages: Informative, precise, human-readable

Provide file, line, nature of the problem, actual vs. expected values.

**Example:**
```
[listToTable.c:101] error : missing -user or -system for task
         [utility/tasks_list:15] task3

[listToTable.c:69] error : wrong task token count
         [utility/tasks_list:19] is 1, should be [2,3]
```

---

## Logging and diagnostics: Code for humans

Use structured, uniform, and meaningful messages for debugging and automation.

**Macro idea:**
```c
#define ERROR_ARG_COUNT(file, line, got, expected_min, expected_max) \
    printf("[%s:%d] arg count error: got %d, expected [%d,%d]\n", \
           file, line, got, expected_min, expected_max);
```

---

## Refactoring mindset

Start by thinking “add more comments,” but end by writing cleaner, clearer code that needs fewer of them.

**Takeaway:**
If you're deleting comments because your code no longer needs them — you're doing it right.
