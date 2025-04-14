# Code Style, Readability, Refactoring & Best Practices Summary

---
## Table of Contents

1. [The Golden Rule of Code](#1-the-golden-rule-of-code)
2. [Readability: Clarity over cleverness](#2-readability-clarity-over-cleverness)
3. [Maintainability: Avoid cryptic names and magic numbers](#3-maintainability-avoid-cryptic-names-and-magic-numbers)
4. [Scalability and Extensibility](#4-scalability-and-extensibility)
5. [Testing and Debugging](#5-testing-and-debugging)
6. [Safety and Robustness](#6-safety-and-robustness)
7. [Tooling and Process](#7-tooling-and-process)
8. [Commenting Rules for Humans](#8-commenting-rules-for-humans)
9. [Naming: Use plural for collections, singular for items](#9-naming-use-plural-for-collections-singular-for-items)
10. [Abbreviations: Use only when standard or obvious](#10-abbreviations-use-only-when-standard-or-obvious)
11. [Error messages: Informative, precise, human-readable](#11-error-messages-informative-precise-human-readable)
12. [Refactoring mindset](#12-refactoring-mindset)
13. [Git Commit Message Guidelines](#13-git-commit-message-guidelines)

---

## 1. The Golden Rule of Code

**Define once. Define clearly. Define where it makes sense.**
Promotes clarity, avoids duplication, and keeps logic in the right layer.

---

## 2. Readability: Clarity over cleverness

Code is read more than it is written. Prioritize understandable variable names and structure.


* Code is for humans first, compilers second.
Don't write clever code. Write code you'd understand a year from now with a hangover.

* Use meaningful names.
status_t file_status; is good. int fs; is not.

* Use typedefs and enums for meaning.
typedef enum { STATUS_OK, STATUS_ERR } status_t; is far clearer than using 0 and 1.

* Be explicit, not implicit.
Prefer if (ptr == NULL) over if (!ptr) when clarity matters.

**Example:**
```c
#define DRIVER_STATUS_DEFAULT (1 << DRIVER_INIT_AT_BOOT) | (1 << DRIVER_START_AT_BOOT)
modules->drivers[i]->status = DRIVER_STATUS_DEFAULT;
```
```c
typeof(table->task_list[i]->status) task_status; // unclear
status_t task_status; // clear
```

---

## 3. Maintainability: Avoid cryptic names and magic numbers

Use meaningful names, constants, and enums instead of hardcoded values.

* Avoid magic numbers.
Use #define TIMEOUT_MS 1000 instead of if(t > 1000).

* Prefer constants over hard-coded values.
const uint8_t MAX_RETRIES = 5; makes intent clear.

* Group related logic.
Keep functions short and focused. One job per function.

* Isolate hardware dependencies.
Wrap I/O in abstraction layers so logic is portable and testable.

**Example:**
```c
fs = 10;                         // bad
file_status = STATUS_DEFAULT;   // good
```

---

## 4. Scalability and Extensibility

Use data structures and algorithms

* Data-driven design beats control-flow spaghetti.
Instead of a long if/else chain, use function pointer arrays, lookup tables, or dispatch maps.

* Avoid duplication.
Copied code multiplies bugs and update work. Refactor common logic into functions.

* Design for change, not perfection.
Anticipate what's likely to change and isolate it.

---
## 5. Testing and Debugging

* Fail early, fail loud.
Use asserts or clear error returns when contracts are broken.

* Log wisely.
Not too little, not too much—log what you need to understand what went wrong.

* Write testable code.
Decouple logic from I/O and hardware to allow mocking.

---

## 6. Safety and Robustness

* Never trust input.
Validate everything—especially in embedded or user-facing code.

* Watch stack and heap usage.
Embedded systems die silently on overflows. Use guards, checks, and tools.

* Defensive coding isn’t paranoia—it’s professionalism.
Check bounds, handle NULLs, and expect the unexpected.

---
## 7. Tooling and Process

* Automate what can be automated.
Builds, tests, formatting, flashing—your time is precious.

* Use version control. Always.
Your future self will thank you.

---

## 8. Commenting Rules for Humans

### 8.1. Comment “why”, not “what”

The code shows what happens. Your job is to write why it happens. Self-documenting code over excessive comments. Write code so clear it needs fewer comments. Use comments to explain **why**, not **what**.

* Don’t: timeout = 1000; // set timeout to 1000
* Do: timeout = 1000; // 1 second max wait before watchdog reset

### 8.2. Explain magic numbers and edge cases

If the value isn't self-explanatory, either use a #define or explain why that number.

* Don’t: if(x == 42)
* Do: if(x == 42) // 42 is the max sensor reading before overflow

### 8.3. Document non-obvious constraints or dependencies

When something works only in a specific sequence, say it.

* Don’t: enableInterrupts();
* Do: enableInterrupts(); // Must be called after initTimer(), otherwise ISR won’t fire

### 8.4. Summarize purpose at the start of non-trivial blocks

tell the reader where they are and what this block is about.

Do:
```c
// Retry logic for unstable connection
for(i = 0; i < MAX_RETRIES; i++)
{
    ...
}
```

### 8.5. Highlight workarounds, hacks, and TODOs

Comments are a dev diary—use them to flag temporary solutions.

Do:

```c
// HACK: force watchdog reset here to avoid rare freeze on power loss
resetWatchdog();

// TODO: replace with dynamic allocation when memory model is fixed
```

### 8.6. Give units when assigning values

Especially in embedded or timing-sensitive code—always say what the units are.

* Don’t: delay = 200;
* Do: delay = 200; // 200 ms delay between measurements

### 8.7. Explain why not something was done

Prevent future bugs by documenting why something wasn’t done.
Do:
```c
// Do not call free() here: pointer is shared across tasks
```

### 8.8. Flag concurrency and timing issues

Race conditions are silent killers. Leave signposts for them.

Do:
```c
// This flag is cleared in the ISR
if(flag_ready)
```

### 8.9. Describe data formats when working with raw buffers

If you’re decoding binary data, draw the structure in the comments.

Do:
```c
// Packet format: [0]=start, [1]=cmd, [2]=len, [3..n]=data, [n+1]=CRC
```

### 8.10. Don’t repeat what the code already says

The golden rule: If a comment just rewords the code, delete it.

* Don’t: index++; // increment index
* Do: index++; // move to next task in round-robin scheduler

### 8.11 Three comment maturity levels

1. Describe what the code does (beginner)
2. Explain the logic’s purpose (intermediate)
3. Refactor code so comments are often unnecessary (advanced)

---

## 9. Naming: Use plural for collections, singular for items

Helps indicate intent and structure of data.

**Example:**
```c
module_t *modules;  // collection
module_t module;    // single item
list_table_t *list_table; // unclear
```

---

## 10. Abbreviations: Use only when standard or obvious

Avoid cryptic short forms unless they are universally recognized (e.g., UART, ISR).
Since autocompletion makes long names easy, there’s no excuse for being cryptic in the name of brevity.

**Example:**
```c
MSGERR();          // unclear
messageError();    // clear
```

---

## 11. Error messages: Informative, precise, human-readable

Provide file, line, nature of the problem, actual vs. expected values. Solution.

* Includes source file + line of the error → Pinpoint precision
* States clearly what went wrong → Not vague
* Shows where the bad data came from → [file]
* Prints the actual offending line/token → No guessing
* Handles all common failure cases → Unknown command, missing flags, name duplication, wrong arg count, ...
* Speaks like a human → Natural and helpful

### Very bad
`driver arg error`

* No file, no line, no values — just frustration.

### Better
`[autoCode.c:59] driver wrong arg count`

* Shows where, and what kind of error. You're no longer blind.

### Top
`[autoCode.c:59] driver wrong arg count, is 1, should be [2,3]`

* Where the problem is
* What went wrong
* Actual vs expected values

**Example:**
```
[listToTable.c:101] error : missing -user or -system for task
         [utility/tasks_list:15] task3

[listToTable.c:69] error : wrong task token count
         [utility/tasks_list:19] is 1, should be [2,3]
```

---

## 12. Refactoring mindset

Start by thinking “add more comments,” but end by writing cleaner, clearer code that needs fewer of them.

**Takeaway:**
If you're deleting comments because your code no longer needs them — you're doing it right.

### Refactoring from chaos to architecture.
Turning a pile of code into a system.

* Naming things with precision
* Choosing data structures that reflect intent
* Writing code that someone else (or future-you) will understand and trust

### Truths of good code architecture:

* It’s invisible when it works. Bad code screams. Good design whispers.
* The goal isn’t just to make it run. It’s to make it changeable without fear.
* Style is structure. Names, indentation, modularity—they aren’t cosmetic. They’re communication.
* You write for readers, not just compilers. Compilers don’t rage-quit. Humans do.

### Key points of refactoring

This is the hard part—but it’s also what makes a system satisfying to build and maintain.

* Abstraction (layers)
* Architecture (structuring algorithms/data flow)
* Polishing (names, docs, clean logic)

---

## 13. Git Commit Message Guidelines

Commit messages must be **short, clear, and action-oriented**.
Each message should begin with a **verb prefix** followed by a short description of the change.

### Accepted prefixes:
- `add` — for new files, features, or logic
- `remove` — for deleted code, features, or cleanup
- `fix` — for bug fixes or corrections
- `change` — for modifications, refactors, or renames
- `upgrade`

### Examples:
- add command parser for task input
- remove unused debug macros
- fix null pointer in tokenizer()
- change task_list to tasks

- Use **imperative mood** (like a commit is a command: "fix", not "fixed")
- Keep it short (ideally under 60 characters)
- Write one logical change per commit
- Avoid vague messages like `update`, `misc`, `changes`




