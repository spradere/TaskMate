# Command tokenizers in TaskMate

Date: 11 October 2026
Version: `v0.32`
Revision: `a0a52133d2301f3bf2d909081e5e395e70c36845`

## Question

Can SCLI reuse the autoCode command tokenizer?

## Problems found in code

- [autoCode's tokenizer](../../srcs/autoCode/tokenizer.c) splits spaces and tabs in place, handles quoted tokens, and returns explicit overflow, quote, and allocation errors. It uses a 256-byte line and grows its token pointer array with `realloc()` on the host.
- [SCLI's tokenizer](../../srcs/system/services/scli.c) splits a 64-byte line in place without allocation or quote handling. It accepts at most four arguments. A fifth returns zero arguments, so the command is discarded without a diagnostic.
- The [date command](../../srcs/system/services/commands/scli_date.c) parses date fields rather than command arguments. No `strtok()` or `strsep()` call appears in current non-legacy C sources.

## Solution

Keep SCLI's tokenizer local with fixed capacities and an explicit overflow result. Decide whether quoted arguments belong to SCLI before changing its syntax. If another task or service needs command tokenization, consider a bounded `tmLibc` API with caller-provided storage. Measure RAM, stack, and cycles before sharing an implementation; these costs have not been measured.
