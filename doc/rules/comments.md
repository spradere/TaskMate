# 💬 Comment Formatting Rules

## Purpose

This document defines the standard formatting and usage rules for comments in TaskMate source files. The goal is to improve file readability and navigation without adding redundant comments or changing code behavior.

Add comments to make the code less cluttered and to explain anything that is not obvious, dangerous or undocumented.

**Golden rule: don't just create visual noise.**

---

## Script files

A boxed comment must be added under the script file's license header to briefly explain what the scripts do, the inputs and outputs.

exemple `arch_include.awk`:

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

Comments should explain intent, constraints, hardware behaviour, timing, or non-obvious ordering. Do not repeat what the code already states. Use short section comments for groups of declarations when it improves scanning:

```c
/*
 * Modules functions
 */
```

Use inline comments for compact hardware meanings, guard names or block:

```c
GPIO_PIN_MODE_OUTPUT_PP, // push-pull mode

#endif // INTERFACES_GPIO_COMMON_H


	// Process the lists in dependency order: errors, init.rc, then tags.
	for( ac_stage_t stage = AC_STAGE_ERRORS; stage < AC_STAGE_COUNT; stage++ )

```

### Over head functions comments

Use the `BSD` style to comment the most important functions; the goal is twofold: to provide information and to make the code more readable. Short functions of 10-20 lines will likely not require descriptive comments if the function and variable names are self-documenting and contains some inline comments. Functions longer than a page will require comments, if only to break up the monotony of the code.

For example, to spell out:

- non-obvious preconditions.
- ownership rules.
- hardware constraints.
- concurrency requirements.

```
/*
 * All major routines should have a comment briefly describing what
 * they do.  The comment before the "main" routine should describe
 * what the program does.
 */
```

---

## Section comments in .c/.h

**Section comments describe the structure of a file**, not the behavior of individual functions or variables. Use section comments to make the organization of a file immediately visible when scrolling through the source.

**Do not create empty sections**. Keep existing meaningful comments unless they conflict with these rules or are clearly redundant.

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
- `Private variables`
- `Private function prototypes`
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

Use this level only when it materially improves readability. Do not use decorative separators for minor subgroups.

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

Header files use the same visual hierarchy but should reflect the structure of a
public or internal interface rather than copy the `.c` layout directly.


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

The purpose of this operation is readability and source navigation, not code
refactoring.
