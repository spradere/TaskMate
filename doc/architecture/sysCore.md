# 🧠 Architecture Note — sysCore

## Historical developments
`sysCore` grew from a single-file prototype into the kernel heart: module database, run-level structures, scheduler, and software time counters. A major refactor moved MCU-specific code out of core into HAL, while keeping core ownership of policy (thread switching, module lifecycle metadata).

After v0.28, `TaskMate.c` moved above sysCore and the repository gained explicit system, user, HAL, and
interface boundaries. Thread/module pointers and include paths were corrected during that transition.
The AVR context switch was then moved progressively into assembly, stack panic handling moved to the
architecture layer, and the scheduler gained a cooperative-yield trigger in addition to its periodic
preemption.

## Current implementation
`boot.c` starts the diagnostic USART, allocates generated driver/thread records and run-level tables,
calls architecture/MCU/board startup hooks, wires GPIO signals, and starts every generated driver in
ascending configured run level. `modules.c` owns a static database containing driver callbacks, four
thread control blocks, fixed stacks, saved stack pointers, software counters, status bytes, and stack
canaries.

The scheduler installs a callback into the 1 ms HAL timer, starts with thread zero, and performs strict
round-robin selection across all generated threads. The naked AVR timer ISR saves context, gives the
saved stack pointer to `tm_schedulerRR()`, installs the returned pointer, and restores context. A separate
10 ms timer callback decrements every non-zero software counter. Cooperative yield accelerates the next
scheduler interrupt; it does not remove a thread from the round-robin set. GPIO mapping/state and the
generated but currently private run-level table also live in sysCore.

## Well-built code and implementation weaknesses
### Strengths
- Thread control blocks, stacks, driver records, and lifecycle tables are statically allocated with no
  runtime heap use.
- Context-switch mechanism is delegated to HAL/AVR code while selection policy remains in sysCore.
- Stack canaries are initialised for every thread and checked during scheduling.
- Separate 1 ms scheduling and 10 ms delay counters provide simple, predictable timing primitives.

### Remaining weaknesses
- Generated run-level thread lists, `current`, and `next` are allocated but never consulted. The scheduler
  cycles through every thread regardless of run level, dead state, or readiness; yielded state is only
  cleared when a thread is selected.
- Module getter/setter functions do not validate indexes.
- Boot special-cases USART before driver allocation/start and does not track init/start/stop state or
  unwind a partial startup failure. The top-level file also contains target-specific experimental code.
- Shared module status, current-thread state, counters, and ISR callbacks have only partial atomicity and
  volatility documentation. There are no priority, deadline, blocking, idle-thread, or overrun semantics.
