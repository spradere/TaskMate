# 🧵 Architecture Note — tasks

## Historical developments
User tasks started as direct test routines in early TaskMate revisions and were progressively normalized into module-style entries (`tasks_init.rc`) managed by autoCode. This enabled deterministic static thread creation and reduced manual startup code.

After v0.28, user code moved out of the system tree into `srcs/user/tasks`, while hardware-target data
moved into `srcs/user/target/test1`. Task declarations became part of the target's typed
`test1_init.rc`, and LED wiring was moved into target configuration. The syscall layer later gained
cooperative yield, although the two example tasks still use their original polling delay loops.

## Current implementation
`test1_init.rc` registers `task1` and `task2` as `RUN_USER` threads. autoCode creates their control blocks,
256-word stacks, initial AVR contexts, names, status bytes, and function pointers. The scheduler includes
both in the same round-robin set as the service threads.

Each example task reserves a message-service channel once, submits a startup message to USART, and then
loops forever. In each loop it toggles its target-defined logical LED through `sc_gpio`, sets its current
thread's software counter to 50, and busy-waits until the 10 ms counter reaches zero. Periodic scheduler
interrupts still preempt the task during that wait.

## Well-built code and implementation weaknesses
### Strengths
- The two tasks are small, deterministic examples with no direct HAL or register access.
- Logical GPIO and message-service APIs demonstrate the intended top-layer dependency direction.
- Generated registration and fixed stacks avoid runtime allocation and startup discovery.
- Their identical shape makes scheduler and GPIO behavior easy to compare on hardware.

### Remaining weaknesses
- Both tasks spin on `sc_threadGetSTC()` and do not use the available cooperative-yield call, so they
  consume every assigned CPU slice while delayed.
- Period, deadline, priority, stack need, and worst-case execution time are not declared or checked; the
  `RUN_USER` status currently does not affect scheduler eligibility.
- Reserved message channels are never released, and startup errors have no retry or fallback behavior.
- Channel handles are writable global variables rather than private task state, and there are no task
  watchdog, overrun, or fault-containment hooks.
