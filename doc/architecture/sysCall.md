# 📞 Architecture Note — sysCall

## Historical developments
As TaskMate layered architecture matured, `sysCall` became the mediation layer between kernel/services/tasks and hardware-oriented implementation. It consolidated thread delay/yield APIs, error catalog access, and GPIO logical operations.

After v0.28, GPIO calls were kept in the dedicated `sc_gpio` façade while the general syscall file was
adapted to the separated sysCore/HAL tree. In August 2026, an explicit cooperative-yield path was added:
a thread can mark itself yielded and request an early scheduler-timer interrupt instead of waiting only
for the next periodic preemption.

## Current implementation
The syscall layer currently has three small API groups:

- `sysCall.c` wraps the current thread's 16-bit software counter in an AVR atomic section, implements
  cooperative yield, and stores one byte of global status flags;
- `sc_gpio.c` delegates logical set/get/toggle operations to the sysCore GPIO table;
- `error.c` owns the generated error catalog and provides message lookup.

`sc_coopYield()` disables interrupts, marks the current module as yielded, reloads the scheduler timer
near its compare point, restores the interrupt state, and waits until the round-robin scheduler clears
the yielded bit when that thread is selected again. The layer does not perform privilege switching or
memory isolation; it is a C API and architectural boundary.

## Well-built code and implementation weaknesses
### Strengths
- Thread-counter access is protected against the timer ISR updating the same 16-bit state on AVR8.
- Thread information and lifecycle APIs validate names/outputs, use RAM/ROM-aware comparison, and
  update run-level state in AVR atomic sections; the scheduler now skips stopped threads.
- Driver count, information, and lifecycle calls use one generated control callback per driver
  instead of exposing private lifecycle functions to services.
- Upper-layer GPIO code uses logical signal types and does not receive physical pin structures.
- The cooperative-yield mechanism reuses the existing scheduler interrupt and adds no dynamic state.

### Remaining weaknesses
- The temporary service HAL bridge and top-level experimental HAL calls still leave the syscall
  boundary incomplete. The transversal `tm_libc` dependency is intentional and is not part of that
  bridge.
- `sc_threadStart()` accepts unvalidated run levels and, when no saved level exists, records the
  supplied level without applying it to status. Repeated `sc_threadStop()` overwrites the saved
  level with `RL_RUN_NONE`.
- GPIO wrappers perform no signal bounds/configuration checks, and error lookup exposes only text
  rather than a complete error contract.
- Cooperative yield assumes task context and a running scheduler, but task/boot/ISR validity is not
  encoded in the API. The timer-load result is ignored and the resume spin has no timeout.
- Driver lifecycle calls collapse all control failures to `bool`, while multi-call status reporting
  is not an atomic snapshot. The declared `sc_flagClear/Set/Get()` API has no implementation.
