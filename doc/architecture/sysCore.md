# 🧠 Architecture Note — sysCore

## Historical developments
`sysCore` grew from a single-file prototype into the kernel heart: module database, run-level structures, scheduler, and software time counters. A major refactor moved MCU-specific code out of core into HAL, while keeping core ownership of policy (thread switching, module lifecycle metadata).

## Current implementation
`sysCore` contains:
- `modules.*`: static driver/thread tables (partly generated) and thread state.
- `tm_scheduler.*`: round-robin context switching with cooperative yield entry.
- `tm_softwareTimeCounter.*`: per-thread delay counters driven by timer ISR callbacks.
- `runLevel.*`: generated run-level allocation metadata (still partially transitional).

Scheduling is preemptive round-robin on timer callback, with context save/restore delegated to HAL arch primitives.

## Well-built code and implementation weaknesses
### Strengths
- Clear kernel responsibilities and static memory model.
- Minimal scheduler fast path suited to small MCUs.
- Thread context abstraction delegated to HAL arch layer (good split).

### Weaknesses (layer leaks / dependency inversion risk)
- Scheduler includes broad HAL headers and direct timer control; policy/mechanism separation is incomplete.
- Run-level data exists but startup path still manually starts drivers in `main`, indicating unfinished layer integration.
- Global mutable module database and raw status bitfields increase accidental coupling risk.
- No priority scheduling / criticality classes yet; determinism remains limited.

## Future improvements for industrial-grade embedded RTOS
Future improvements should focus on Complete run-level state machine and remove transitional boot loops; Introduce priority-based scheduler with bounded preemption latency analysis; Add kernel tracing hooks (context switch time, ISR-to-task latency, jitter stats); Harden shared state with clear atomic sections and documented lock hierarchy, and Define strict kernel ABI to decouple sysCore internals from sysCall consumers.
