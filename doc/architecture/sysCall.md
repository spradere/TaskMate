# 📞 Architecture Note — sysCall

## Historical developments
As TaskMate layered architecture matured, `sysCall` became the mediation layer between kernel/services/tasks and hardware-oriented implementation. It consolidated thread delay/yield APIs, error catalog access, and GPIO logical operations.

## Current implementation
`sysCall` currently groups:
- thread timing/yield helpers,
- global status flags,
- error catalog access,
- logical GPIO operations (via `sc_gpio`).

It remains intentionally thin and delegates stateful policy to sysCore.

## Well-built code and implementation weaknesses
### Strengths
- Single API surface for upper layers reduces direct kernel/HAL coupling.
- Keeps call paths lightweight for constrained targets.
- Pairs naturally with interface-defined data contracts.

### Remaining weaknesses
- Domain mixing in one layer (thread + GPIO + diagnostics) can still grow monolithic.
- Some wrappers are pass-through and do not enforce usage policy strongly.
- Context-safety contracts (task vs ISR-safe APIs) are not fully formalized.

## Future improvements for industrial-grade embedded RTOS
- Split syscalls by domain (`sc_thread`, `sc_io`, `sc_diag`) with explicit context guarantees.
- Add structured return/error policies (beyond message lookup).
- Add syscall tracing/audit hooks for runtime observability and misuse detection.
