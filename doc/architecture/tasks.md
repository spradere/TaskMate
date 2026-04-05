# 🧵 Architecture Note — tasks

## Historical developments
User tasks started as direct test routines in early TaskMate revisions and were progressively normalized into module-style entries (`tasks_init.rc`) managed by autoCode. This enabled deterministic static thread creation and reduced manual startup code.

## Current implementation
Example tasks (`task1`, `task2`) follow a simple periodic pattern:
- optional service message,
- logical GPIO toggle through syscall API,
- delay/yield behavior driven by software time counters.

Registration and stack allocation are static and generated.

## Well-built code and implementation weaknesses
### Strengths
- Minimal template keeps behavior readable and reproducible.
- Static registration avoids runtime allocation variance.
- Uses stable top-layer APIs (services + syscalls).

### Remaining weaknesses
- Timing intent is still implicit in code loops rather than declarative contracts.
- Busy-wait style delay patterns remain in examples and can waste CPU budget.
- No built-in task-level deadline/overrun handling semantics yet.

## Future improvements for industrial-grade embedded RTOS
- Add declarative task metadata (period/deadline/WCET/criticality).
- Prefer blocking sleep primitives over polling loops.
- Add watchdog/overrun policy hooks and static task API-usage checks.
