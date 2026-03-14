# 🧵 Architecture Note — tasks

## Historical developments
User tasks started as direct test routines in early TaskMate revisions and were progressively normalized into module-style entries (`tasks_init.rc`) managed by autoCode. This enabled deterministic static thread creation and reduced manual startup code.

## Current implementation
Tasks (`task1`, `task2`) are simple periodic loops:
- optional startup message through service channel,
- GPIO signal toggling,
- delay through software time counter polling.

They are generated into thread tables with run-level/user status metadata and dedicated stacks.

## Well-built code and implementation weaknesses
### Strengths
- Very small task template, easy to replicate and reason about.
- Static allocation and generated registration eliminate dynamic registration risks.
- Demonstrates clean usage of sysCall and service APIs.

### Weaknesses (layer leaks / dependency inversion risk)
- Tasks can still include service and syscall APIs freely; no contract for allowed runtime behavior.
- Busy-wait delay loops (`while(STC>0);`) can waste CPU and increase jitter.
- No deadline or period declaration at API level; timing intent is implicit in code.
- Global task variables (message channels) are not namespaced per instance model.

## Future improvements for industrial-grade embedded RTOS
- Future improvements should focus on Add declarative task model: period, deadline, WCET budget, priority/criticality
- Replace polling waits with scheduler-managed sleep/block primitives
- Introduce task watchdog and overrun handling policies, and Provide static analysis hooks verifying task API usage constraints.
