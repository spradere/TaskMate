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
- Upper-layer GPIO code uses logical signal types and does not receive physical pin structures.
- The cooperative-yield mechanism reuses the existing scheduler interrupt and adds no dynamic state.
- APIs remain small and inexpensive on the current target.

### Remaining weaknesses
- The boundary is incomplete: services and the top-level startup still call HAL directly, while
  `tm_libc` calls `sc_coopYield()` from below the syscall layer.
- GPIO wrappers perform no signal bounds/configuration checks, and error lookup exposes only text rather
  than a complete error contract.
- Cooperative yield assumes task context and a running scheduler, but task/boot/ISR validity is not
  encoded in the API. The spin after requesting a switch has no explicit timeout or misuse detection.
