# 🪶 TaskMate Coding Style Rules

**This guide documents the style used in the TaskMate codebase. It complements the existing `.clang-format`, `.clang-tidy`, architecture rules, and prefix rules.**
 
---

## Formatting

Use the project `.clang-format` file as the source of truth for C and header formatting.

- Indent with tabs. Tab width and indentation width are both 4.
- Use Allman braces:

```c
if( condition )
{
	doWork();
}
```

- Keep the project spacing style around control statements:
  `if( condition )`, `for( ... )`, `while( ... )`.
- Short simple bodies may stay on one line when this improves readability:

```c
if( value == 0 ) { return ERR_NO_ERROR; }
```

- Keep lines at or below 100 columns.
- Use right-aligned pointer stars:

```c
const tm_string_t *name;
mod_thread_item_t *thread;
```

- Prefer explicit parentheses in conditions and macros when operator precedence
  could be unclear.

---

## File Structure

Start C, header, scripts, and Makefile fragments with the TaskMate BSD-2-Clause. Add a Doxygen file block after the license banner in C and header files:

```
/**
 * @file module.c
 * @brief module implementation.
 */
```

- Include the matching local header first in `.c` files.
- After the matching header, include system headers before project headers.
- Let clang-format sort include blocks.
- Build each header guard from the immediate parent directory and the file name. Convert both
  names to uppercase, preserve word-separating underscores, replace other separators with
  underscores, omit the file extension, and use `<LAST_PARENT_DIRECTORY>_<FILE_NAME>_H`:

```c
#ifndef SYSCORE_SYS_MODULES_H
#define SYSCORE_SYS_MODULES_H

...

#endif // SYSCORE_SYS_MODULES_H
```

---

## Naming

Use the established TaskMate prefixes only when they describe a real boundary or subsystem. Follow `doc/rules/TaskMate_prefixes.md` and `doc/rules/make_prefixes.md`.

- Public subsystem APIs use lower camel case after the prefix.
- Types use snake case with a `_t` suffix.
- Enum constants and preprocessor constants use uppercase snake case.
- Variables uses descriptive snake case.

---

## C Design Rules

**Specifics of embedded code:**

- Mandatory usage of explicit fixed-width integer types from `<stdint.h>` for embedded data.
- Use `bool` from `<stdbool.h>` for boolean state.
- Keep hardware-specific code inside HAL architecture.
- Keep portable contracts in `srcs/interfaces/`; they must not depend on HAL,sysCall, services, or tasks.
- Tasks should use sysCall and service APIs, not raw HAL or MCU registers.
- Keep module state private with `static` file-local storage unless it is part of an intentional public interface.
- Prefer small focused functions over large mixed-responsibility functions.
- Avoid magic numbers in hand-written code. Use named constants or document why a literal is required.
- Use explicit null or zero checks when this makes intent clearer.
- For register macros and bit manipulation, fully parenthesize macro parameters and results.

---

## Generated Code

**Do not manually edit generated regions.** Generated regions are marked with autoCode tags :

```c
// [autoCode_tag] name
// clang-format off
#include "files.inc"
// [/tag]
```

---

## Build Scripts

- Keep BSD license banners on Makefile fragments and scripts.
- Use uppercase semantic Make variables and `${VAR}` expansion consistently.
- Prefix shell commands in make recipes with `@` when command echoing would add
  noise to normal build output.
- Use aligned, readable `printf` output for build summaries and diagnostics.
- In AWK scripts, keep parser state explicit with named state variables and
