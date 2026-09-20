# 🧠 Architecture Note — sysCore

## Historical developments
`sysCore` grew from a prototype into module storage, scheduling, and software-time ownership.
MCU mechanisms moved to HAL while context switching became architecture-specific.
After `v0.28`, generated records and stack canaries defined the static module database.
Commit `db59169` made scheduling respect the active run level.
Commits `221e69b` and `1e2fd4d` clarified sysCore file ownership and moved names to sysCall.
Before `v0.31`, GPIO left sysCore and context operations moved behind neutral interfaces.

## Current implementation
sysCore owns static driver and thread records, current-thread state, saved contexts, per-thread
software counters, stack canaries, the active run level, and round-robin selection policy.

Boot allocates generated records, connects the software-counter callback, initializes scheduling,
and starts thread zero. The scheduling timer saves the current opaque context, checks both canaries,
and chooses the next thread whose non-zero run level does not exceed the active level.

The system service advances run levels through sysCall after readiness checks. A separate periodic
timer decrements software counters. Cooperative yield marks the caller and requests an early
scheduling interrupt; sysCore contains no tmLibc or concrete HAL-header dependency.

## Well-built code and implementation weaknesses
### Strengths
- Driver, thread, context, stack, and timer state are static; firmware uses no runtime heap.
- Context mechanics remain in HAL while admission and selection policy remain in sysCore.
- Active run levels prevent later services and tasks from running during staged startup.
- Both stack canaries are checked before and after every scheduling decision.

### Remaining weaknesses
- Eligible threads are equal peers; dead and initialized bits do not affect selection.
- There is no priority, blocking, deadline, idle-thread, watchdog, or overrun policy.
- Module pointer access trusts indices and current-thread state lacks an explicit bounds contract.
- Fixed equal stacks have no measured high-water reporting or hardware-validated margin.
