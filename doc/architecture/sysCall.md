# 📞 Architecture Note — sysCall

## Historical developments
As TaskMate layered architecture matured, `sysCall` became the mediation layer between kernel/services/tasks and hardware-oriented implementation. It consolidated target info exposure, thread delay/yield APIs, error catalog access, and GPIO logical operations.

## Current implementation
`sysCall` currently provides:
- target metadata (`sc_targetGetInfo`), generated from build options,
- thread timing helpers (`sc_threadSetSTC`, `sc_threadGetSTC`, `sc_handYield`),
- global status flags,
- error catalog API (`err_getMessage`),
- GPIO logical signal API (`gpio_signal*`).

It acts as a thin wrapper layer over `sysCore` and HAL/user-facing generated headers.

## Well-built code and implementation weaknesses
### Strengths
- Centralized system-facing API for upper layers.
- Lightweight wrappers keep code size low.
- Error and target metadata centralization improves diagnostics consistency.

### Weaknesses (layer leaks / dependency inversion risk)
- Several syscalls are near-pass-through helpers; policy remains spread in callers.
- `sc_flagGet` implementation mutates status during read (`&=`), likely unintended and risky.
- GPIO syscall directly mirrors HAL calls, exposing low-level semantics without permission/ownership model.
- Mixed concerns (target info, thread control, GPIO, error catalog) in one layer can become monolithic.

## Future improvements for industrial-grade embedded RTOS
- Future improvements should focus on Split syscalls into domain modules (thread, io, diagnostics, platform info)
- Add privilege-like API contracts (which context can call what, ISR-safe variants)
- Define non-blocking/error-reporting conventions suitable for safety-critical flows, and Introduce syscall tracing and audit hooks for runtime observability.
