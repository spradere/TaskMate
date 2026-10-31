# Architecture Note: tasks

## Historical developments
User tasks began as direct test routines before autoCode gave them fixed records and stacks.
After `v0.28`, task code and target wiring moved into separate source trees.
Tag `v0.31` marks initialization acknowledgement, run-level admission, and generated source selection.
After `v0.32`, task stacks became target-configured and the AVR demonstration stacks were reduced after measurement.
After `v0.32`, build rules and public headers received stronger boundary and API checks.

## Current implementation
The current targets register `task1` and `task2` at user run level. Each receives a generated static record, target-sized stack, initial context, name, status, saved run level, counter, and entry callback.

The reference `test1` target assigns 100-byte stacks to both tasks. The scheduler excludes them until startup reaches user level. Each task then declares readiness, toggles a target-defined LED, loads a 50-tick software counter, and spins until it expires.

Tasks include their local header and sysCall APIs. They may use services and tmLibc, but architecture checks prohibit direct interfaces, sysCore, or concrete HAL dependencies.

## Well-built code and implementation weaknesses
### Strengths
- Demonstration tasks have deterministic static records and no direct hardware dependency.
- Logical GPIO follows the intended task to sysCall to neutral HAL path.
- Generated registration, target-configured stacks, and fixed timing metadata avoid runtime discovery and allocation.
- Tasks acknowledge readiness and cooperatively yield during software-counter delays.

### Remaining weaknesses
- Tasks declare delay and WCET values, but deadlines, priorities, and enforcement remain absent.
- User run level controls admission but provides no distinct scheduling policy afterward.
- Delay loops repeatedly poll a shared counter and yield instead of using a blocking timer wait.
- Two synthetic LED tasks provide little evidence for realistic workload, overload, or inter-task communication behaviour.
