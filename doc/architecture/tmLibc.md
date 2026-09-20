# 📚 Architecture Note — tmLibc

## Historical developments
`tmLibc` was introduced for bounded, small-footprint strings, formatting, and logging.
After `v0.28`, AVR RAM/ROM descriptors supported constant text without dynamic allocation.
Commit `a0b1a51` defined tmLibc as a horizontal layer above sysCall rather than transversal code.
Commits `9c64446` and `58958b8` moved storage access and console transport behind sysCall.
Commit `94ff337` grouped formatting below `tmLibc/stdio/`.
Before `v0.31`, string macros became target-selected without a `hal/public` relay.

## Current implementation
Build options select TaskMate implementations or partial standard-library aliases. TaskMate mode
provides bounded copy and comparison plus compact formatting for characters, strings, integers,
hexadecimal, binary, percent, and one-digit zero padding.

tmLibc consumes storage-aware descriptors and calls sysCall to read bytes and transport console
output. On AVR8, build-selected macros place constant text in program memory while RAM buffers are
wrapped explicitly. Tasks and services may use the layer; HAL, sysCore, and sysCall may not.

Formatting uses fixed static state and no heap. Boot no longer depends on tmLibc: early USART status
bytes are emitted through the neutral driver contract before scheduling and normal service logging.

## Well-built code and implementation weaknesses
### Strengths
- Constant strings reduce AVR RAM use while callers retain a storage-neutral descriptor API.
- Copy, comparison, and formatting bound accesses and handle null text defensively.
- The formatter has a fixed feature set, static storage, and no allocation.
- Architecture checks enforce the downward tmLibc to sysCall and interfaces dependency direction.

### Remaining weaknesses
- Shared formatter state makes formatting and logging non-reentrant and unsafe across preemption.
- Return length and truncation behaviour differ from standard `snprintf` expectations.
- The standard-library branch is incomplete and does not match descriptor-based signatures.
- Backend macros are compiler-injected, and RAM/ROM details remain visible in public call sites.
