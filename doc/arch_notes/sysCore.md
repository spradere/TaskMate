# Architecture Note: sysCore

## Historical developments
`sysCore` grew from a prototype into static module storage, scheduling, and software-time ownership.
After `v0.28`, generated records and stack canaries defined the static module database.
Tag `v0.31` marks run-level admission, removal of GPIO ownership, and neutral context mechanisms.
After `v0.32`, the module database split into dedicated driver and thread units.
After `v0.32`, one timer-context callback combined preemption with software-counter ticks.
After `v0.32`, stack fill patterns and depth measurement supplemented boundary canaries.

## Current implementation
sysCore owns separate static driver and thread records, current-thread state, saved contexts, software counters, stack fill and canary data, the active run level, and round-robin policy.

Boot allocates generated records and starts the scheduler. The timer-context callback saves the current opaque context, checks both canaries, advances all software counters every ten interrupts, and selects the next thread whose non-zero run level is active.

The system service advances run levels through sysCall after readiness checks. Cooperative yield requests an early timer-context event. Stack-depth queries scan the untouched fill pattern while protected by the syscall atomic section.

## Well-built code and implementation weaknesses
### Strengths
- Driver, thread, context, stack, and counter state are static and use no runtime heap.
- Context mechanics remain in HAL while admission and round-robin policy remain in sysCore.
- Active run levels prevent later services and tasks from running during staged startup.
- Boundary canaries and measurable fill patterns expose stack overflow and high-water information.

### Remaining weaknesses
- Scheduling ignores initialized, dead, and yielded status when deciding whether a thread is runnable.
- There is no priority, blocking, deadline, idle-thread, watchdog, or overrun policy.
- Internal pointer access trusts IDs and the current-thread index without local bounds checks.
- Canary failure halts without identifying the thread or preserving diagnostic context.
