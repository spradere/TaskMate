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
state. The public HAL backend reads stored text and buffers USART output until full or newline.

The diagram and dependency matrix now make `tmLibc` available to tasks and services, with only
`sysCall` and `interfaces` below it. It is no longer transversal: HAL, sysCore, and `sysCall` must
not depend on it, and it must not call HAL or sysCore.

The source has not migrated yet. `tmLibc` still includes public HAL support, while sysCore,
`sysCall`, and top-level startup still consume its string or logging facilities. These are recorded
migration edges, not the target architecture.

## Well-built code and implementation weaknesses
### Strengths
- Program-memory strings reduce scarce AVR RAM use.
- Bounded string operations handle null text, capacity, and termination explicitly.
- Formatting uses a fixed feature set and temporary storage with no heap.
- Invalid padding leaves through the common formatter cleanup path.

### Remaining weaknesses
- Direct libc-to-HAL and lower-layer-to-libc dependencies still violate the target direction.
- Formatting uses shared state and a non-atomic lock; one yield does not guarantee ownership.
- Small buffer capacities can be mishandled, and return length differs from standard `snprintf`.
- The standard-libc branch is incomplete, while logging lacks level, sink, and timing policy.
