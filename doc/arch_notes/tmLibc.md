# Architecture Note: tmLibc

## Historical developments
`tmLibc` was introduced for bounded, small-footprint strings, formatting, and logging.
After `v0.28`, RAM and ROM descriptors supported AVR constant text without dynamic allocation.
Tag `v0.31` marks tmLibc as a layer above sysCall, with storage access and console transport below it.
After `v0.32`, public APIs gained explicit contracts and formatter constants replaced numeric literals.
After `v0.32`, low-level storage and console byte paths standardized on `uint8_t`.

## Current implementation
Build options select TaskMate implementations or partial standard-library aliases. TaskMate mode provides bounded copy and comparison plus compact formatting for characters, strings, unsigned integers, hexadecimal, binary, percent, and one-digit zero padding.

tmLibc consumes storage-aware descriptors and calls sysCall to read bytes and transport console output. On AVR8, selected macros place constant text in program memory; the host uses ordinary constant storage. Tasks and services may use the layer, while HAL, sysCore, and sysCall may not.

Formatting uses one fixed static state object and no heap. A null destination writes to the console and flushes on newline. Boot remains independent of tmLibc.

## Well-built code and implementation weaknesses
### Strengths
- Constant strings reduce AVR RAM use while callers retain one descriptor API.
- Copy, comparison, and formatting bound destination accesses, handle null text, preserve unsigned width, and report transport failures.
- TaskMate and standard-library backends now expose backend-appropriate string types and headers.
- Architecture checks enforce the downward tmLibc to sysCall and interfaces dependency direction.

### Remaining weaknesses
- Shared formatter state makes formatting and logging non-reentrant across threads, preemption, or interrupts.
- The formatter supports only a small conversion subset and silently drops unsupported specifiers.
- Backend selection changes public string types and formatting behaviour between targets.
- Target string macros are compiler-injected, and RAM versus ROM storage remains visible at TaskMate-libc call sites.
