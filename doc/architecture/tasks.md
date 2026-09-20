# 🧵 Architecture Note — tasks

## Historical developments
User tasks began as direct test routines before autoCode gave them fixed records and stacks.
After `v0.28`, task code and target wiring moved into separate source trees.
Commit `830116e` added task initialization acknowledgement.
Commit `db59169` gated task admission on the active user run level.
The source-selection refactor now attaches each task translation unit to its `init.rc` declaration.
At `v0.31`, `test1` remains the sole target and contains two demonstration tasks.

## Current implementation
autoCode registers `task1` and `task2` at the user run level with fixed 256-byte AVR stacks,
initial contexts, generated names, status, saved run levels, and entry callbacks.

The scheduler excludes both tasks until staged startup reaches the user level. Each task then marks
itself initialized, toggles its target-defined logical LED, loads a 50-tick software counter, and
busy-waits while periodic scheduling interrupts continue to preempt it.

Tasks include only their local API plus sysCall headers. They may use service and tmLibc APIs, but
must not include neutral interfaces, sysCore, concrete HAL headers, or MCU registers directly.

## Well-built code and implementation weaknesses
### Strengths
- The examples have deterministic static records and no direct hardware dependency.
- Logical GPIO follows the intended task to sysCall to neutral HAL contract path.
- Generated registration and fixed stacks avoid runtime discovery and allocation.
- Run-level admission and initialization acknowledgement integrate tasks into startup.

### Remaining weaknesses
- Period, deadline, priority, stack need, and worst-case execution time are not declared or checked.
- User level controls admission but provides no distinct scheduling policy afterward.
- Busy-wait delays consume every assigned slice instead of using cooperative yield or blocking.
- Two synthetic LED tasks provide little evidence for real workload or overload behaviour.
