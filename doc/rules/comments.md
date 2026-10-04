# 💬 Comment Formatting Rules

## Purpose

This document defines the formatting and usage rules for comments in TaskMate source files. The goal is to improve readability and navigation without adding redundant comments or changing code behavior.

Add comments to explain intent, constraints, and behavior that is not obvious from the code.

**Golden rule: don't just create visual noise.**

---

## Script files

A boxed comment must follow a script file's license header. It briefly describes the script, its inputs, and its outputs.

Example from `arch_include.awk`:

```
# ------------------------------------------------------------------------------
# Description: Validate direct includes against the architecture matrix.
#
# Inputs: Matrix and source files, with matrix_file and optional path_sources variables.
# Outputs: Validation summary or violations on stdout, with a nonzero status on failure.
# ------------------------------------------------------------------------------
```

---

## Comments in code

Comments should explain intent, constraints, hardware behavior, timing, or non-obvious ordering. Do not repeat what the code already states.

Use short comments to mark the main steps of a longer function. Place each comment before the block it explains. For example, `tokenizer.c` separates skipping whitespace, handling quoted tokens, growing the token table, and terminating a token. Describe the purpose of a step, not each statement inside it.

In long functions, aim for one inline comment per 10 lines of code. A gap of 5 to 20 lines between comments is a useful range when the code has distinct operations. Follow the logical blocks rather than a fixed quota; do not add comments that only restate the code.

Explain data ownership and side effects when they are easy to miss. In `tokenizer.c`, tokens point into the input line, and the parser terminates each token in that line. A short comment at that point helps readers understand why the line is modified.

Comment non-obvious error handling or ordering, such as cleaning up allocated tokens after an error. Keep comments short and update them when the behavior changes. Do not add a comment to every function or repeat a clear function name.

Use short section comments for groups of declarations when they improve scanning:

```c
/*
 * Module declarations
 */
```

Use end-of-line comments sparingly for compact hardware meanings or guard names. Put a comment before a block when it explains the whole block:

```c
GPIO_PIN_MODE_OUTPUT_PP, // push-pull mode

#endif // INTERFACES_GPIO_COMMON_H

	// Process the lists in dependency order: errors, init.rc, then tags.
	for( ac_stage_t stage = AC_STAGE_ERRORS; stage < AC_STAGE_COUNT; stage++ )
```

### Function comments

Use BSD-style comments for important functions; the goal is twofold: to provide information and to make the code more readable. Short functions of 10-20 lines will likely not require descriptive comments if the function and variable names are self-documenting and contains some inline comments. Functions longer than a page will require comments, if only to break up the monotony of the code. Longer functions may need comments to mark their main steps.

Use function comments to explain:

- Non-obvious preconditions.
- Ownership rules.
- Hardware constraints.
- Concurrency requirements.

```c
/*
 * All major routines should have a comment briefly describing what
 * they do.  The comment before the "main" routine should describe
 * what the program does.
 */
```

### Doxygen in headers

Add Doxygen comments to public structures, enums, functions, constants, and macros in headers. Keep existing `@file` comments as specified by `doc/rules/style.md`. Do not document private declarations, header guards, generated regions, or individual structure fields.

- Place a structure or enum comment immediately before `typedef` or the named declaration.
- Use a one-sentence `@brief` that states the purpose or contract. Do not repeat the identifier in prose.
- Group related public `#define` declarations with one brief Doxygen `@name` block and `@{` / `@}` markers. Use an individual `@brief` only when a macro has a distinct contract.
- Keep implementation helpers and public macros in separate groups when the distinction matters to callers.
- For functions, add `@param` for each parameter when its role, units, valid range, ownership, or output behavior needs explanation. Use `[out]` or `[in,out]` where appropriate.
- Add `@return` when the return value needs interpretation, especially for status codes and boolean results. State what each result means without restating the return type.
- Mention timing, interrupt context, initialization order, or other constraints only when they affect correct use. Keep each description brief and precise.
- Document the public contract in the header. Avoid repeating the same Doxygen block in the `.c` definition.

```c
/**
 * @brief Selects RAM or ROM storage for a string.
 */
typedef enum
{
	TM_MEM_RAM,
	TM_MEM_ROM
} tm_string_storage_t;

/**
 * @brief Identifies string data and its storage location.
 */
typedef struct
{
	const char *text;
	const tm_string_storage_t storage;
} tm_string_t;

/**
 * @brief Read one byte from the USART receive buffer.
 * @param[out] data Receives the byte when the call succeeds.
 * @return ERR_NO_ERROR if a byte was read; an error code otherwise.
 */
err_codes_t sc_usartRead(uint8_t *data);
```

```c
/**
 * @name Run level encoding
 * @brief Bound run levels and extract their status bit field.
 * @{
 */
#define RL_LEVEL_MASK 0x07
#define RL_LEVEL_COUNT 5
/** @} */
```

---

## Section comments in .c/.h

**Section comments describe the structure of a file**, not the behavior of individual functions or variables. Use them to make the organization of a file visible when scrolling.

**Do not create empty sections.** Keep meaningful comments unless they conflict with these rules or are clearly redundant.

TaskMate uses three visual levels:

---

### Level 1 - Major file section

Use `=` separators for major structural parts of a file.

```c
/* =============================================================================
 * Declarations
 * ===========================================================================*/
```

Typical major sections include:

- `Declarations`
- `Implementation`
- `Public API`

Major sections should be used sparingly.

---

### Level 2 - Functional group

Use `-` separators for groups inside a major section.

```c
/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/
```

Typical groups include:

- `Constants`
- `Private types`
- `Private variables / functions`
- `Macros`
- `Types`
- `Driver lifecycle / operations`
- `Error handling`

Names should describe the role of the following code, not merely its syntax.

---

### Level 3 - Minor subgroup

For small subdivisions inside a functional group, use a simple comment.

```c
/* 
 * Scan operations 
 */
```

Use this level only when it improves readability. Do not use decorative separators for minor subgroups.

---

### C source files recommended structure

A `.c` file should normally follow this general organization when applicable:

```c
/*
 * TaskMate Project
 * ...
 */

/**
 * @file example.c
 * @brief ...
 */

/* ============================================================================
 * Declarations - Include
 * ========================================================================== */

#include "example.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

/* -----------------------------------------------
 * Private types
 * ---------------------------------------------*/

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

/* ============================================================================
 * Implementation - Functions
 * ========================================================================== */

/* -----------------------------------------------
 * Functional group 1
 * ---------------------------------------------*/
```

Only sections containing actual content should be present.

---

### Header files recommended structure

Header files use the same visual hierarchy but should reflect the structure of a public or internal interface rather than copy the `.c` layout directly.


```c
/*
 * TaskMate Project
 * ...
 */

/**
 * @file example.h
 * @brief ...
 */

#ifndef EXAMPLE_H
#define EXAMPLE_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include ...

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

/* -----------------------------------------------
 * Types
 * ----------------------------------------------*/

/* ------------------------------------------------
 * Macros
 * ----------------------------------------------*/

/* ============================================================================
 * Public API
 * ========================================================================== */

void anyFunction(void);
...

#endif
```

---

## Codex application rules

When asked to apply this document to existing TaskMate files:

1. Read the complete target file before modifying it.
2. Identify the existing logical structure.
3. Add or normalize section comments according to this document.
4. Preserve useful existing documentation comments.
5. Remove only structural comments made redundant by the new organization.
6. Do not create empty sections.
7. Group implementation functions according to logical responsibility.
8. Avoid moving functions unless necessary and safe.
9. Prefer comment-only changes.
10. Do not alter code behavior.
11. Do not perform unrelated style cleanup.
12. If the correct logical grouping is ambiguous, ask user.

The purpose of this operation is readability and source navigation, not code refactoring.
