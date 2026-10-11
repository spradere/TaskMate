# Singular and plural names in TaskMate

Date: 11 October 2026
Version: `v0.32`
Revision: `a0a52133d2301f3bf2d909081e5e395e70c36845`

## Question

Do singular and plural names in the current tree identify a naming problem that requires renaming?

## Problems found in code

- Singular names such as `target/`, `board/`, and `system/` identify a category or layer. Plural names such as `tasks/`, `services/`, `commands/`, and [hal/drivers/](../../srcs/hal/drivers/) identify collections. The distinction is generally clear.
- [sc_threads.c](../../srcs/system/sysCall/sc_threads.c) covers a collection; [sc_gpio.c](../../srcs/system/sysCall/sc_gpio.c) names a domain. The existing [file naming rule](../rules/file_name.md) asks for descriptive names and preservation of subsystem style, with no singular-only rule.
- A directory rename would affect `init.rc` source paths, Make fragments, boundary checks, and tests. No functional defect follows from the current spelling.

## Solution

Use singular names for new layers, categories, instances, and focused files. Use plural names when a collection is the clearest meaning. Preserve established names that already describe their role clearly.
