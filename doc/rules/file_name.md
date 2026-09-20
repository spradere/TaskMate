# TaskMate File Naming Rules

## Purpose

This document defines file and directory naming rules for TaskMate source code
and build inputs. Names must remain predictable and safe for POSIX tools, shell
scripts, Makefiles, and cross-platform builds.

## Rule 1 — Use the POSIX portable filename character set

Every file and directory name must use only:

- uppercase ASCII letters: `A-Z`;
- lowercase ASCII letters: `a-z`;
- decimal digits: `0-9`;
- period: `.`;
- underscore: `_`;
- hyphen: `-`.

Do not use spaces, tabs, control characters, non-ASCII characters, or shell
metacharacters in names. Contributors who require spaces must handle that need
outside the TaskMate tree.

## Rule 2 — Avoid ambiguous or unsafe names

- Do not begin a file or directory name with `-`; command-line tools may treat
  it as an option.
- Do not use `.` or `..` as a name.
- Do not rely on letter case alone to distinguish two paths.
- Use exactly one conventional extension where the file type has one.
- Keep names concise and descriptive; do not encode temporary status or author
  names in them.

## Rule 3 — Source-code files

- Preserve the established subsystem naming style when naming C source and
  header files.
- Give a C implementation and its matching header the same basename, for
  example `sys_time.c` and `sys_time.h`.
- Use lowercase extensions: `.c`, `.h`, `.s`, and `.inc`.
- Name configuration inputs according to their established format and retain
  their lowercase extension, such as `.rc` and `.err`.
- Do not introduce whitespace as a substitute for word boundaries; use the
  naming form already established by the owning subsystem.

## Rule 4 — Build files and scripts

- All names referenced by Makefiles, shell scripts, or AWK scripts must comply
  with these rules, including generated files and target-specific paths.
- Use lowercase extensions for scripts and Make fragments, such as `.sh`,
  `.awk`, and `.mk`.
- Keep the conventional names `Makefile` and `BSDmakefile` when applicable.
- Do not depend on quoting or escaping to make a non-compliant pathname work.
- Generated pathnames must be validated or constructed so that they obey the
  same rules as repository pathnames.

## Examples

Valid names:

```text
sys_time.c
path_files.mk
atmega2560
TaskMate_prefixes.md
```

Invalid names:

```text
system time.c
-debug.mk
driver@board.c
résumé.md
```
