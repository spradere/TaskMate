# 📚 Architecture Note — tmLibc

## Historical developments
`tmLibc` was introduced to bound code size and behaviour on constrained targets. It gained compact
formatting, RAM/ROM-aware strings, and logging through target-specific output backends.

After tag `v0.28`, it was treated as transversal in the separated tree. Commit `ec88d98` corrected
formatter behaviour; later revisions added a cooperative-yield attempt for contention.

Tag `v0.29` marks the baseline before the current syscall and startup refactors.
Commit `a0b1a51` then redefined it as a horizontal layer above `sysCall`.

## Current implementation
Build options select either TaskMate implementations or partial standard-library aliases. TaskMate
mode provides bounded copy and comparison plus compact print functions supporting characters,
strings, integers, hexadecimal, binary, percent, and one-digit zero padding.

Text descriptors distinguish RAM from AVR program memory. Formatting and logging share fixed static
state. `sysCall` reads stored bytes and transports USART output in bounded chunks.

The dependency matrix makes `tmLibc` available to tasks and services, with only `sysCall` and
`interfaces` below it. It is no longer transversal: HAL, sysCore, and `sysCall` do not depend on it,
and it does not call HAL or sysCore. Top-level startup still uses logging before scheduling.

## Well-built code and implementation weaknesses
### Strengths
- Program-memory strings reduce scarce AVR RAM use.
- Bounded string operations handle null text, capacity, and termination explicitly.
- Formatting uses a fixed feature set and temporary storage with no heap.
- Invalid padding leaves through the common formatter cleanup path.

### Remaining weaknesses
- Early startup logging still couples top-level boot sequencing to the library.
- Formatting uses shared state and a non-atomic lock; one yield does not guarantee ownership.
- Small buffer capacities can be mishandled, and return length differs from standard `snprintf`.
- The standard-libc branch is incomplete, while logging lacks level, sink, and timing policy.
