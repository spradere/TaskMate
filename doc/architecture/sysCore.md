# 🧠 Architecture Note — sysCore

## Historical developments
`sysCore` grew from a single-file prototype into the kernel heart: module database, run-level structures, scheduler, and software time counters. A major refactor moved MCU-specific code out of core into HAL, while keeping core ownership of policy (thread switching, module lifecycle metadata).

## Current implementation
`sysCore` owns kernel policy and state:
- module/thread metadata (`modules.*` + generated lists),
- scheduler (`tm_scheduler.*`),
- software time counters (`tm_softwareTimeCounter.*`),
- boot/run-level progression (`boot.*`, `runLevel.*`).

HAL is used as a mechanism provider (context/timers/GPIO primitives), while sysCore keeps scheduling and lifecycle decisions.

## Well-built code and implementation weaknesses
### Strengths
- Core responsibilities are well-centered around scheduling, lifecycle, and module metadata.
- Static memory model remains deterministic and embedded-friendly.
- Dependency cleanup improved clarity between policy (sysCore) and shared definitions (`interfaces`).

### Remaining weaknesses
- Scheduler policy is still simple round-robin with limited timing-class controls.
- Transitional startup/run-level flow still mixes generated and handwritten orchestration.
- Shared mutable global state needs stronger concurrency/atomicity documentation.

## Future improvements for industrial-grade embedded RTOS
- Finalize run-level state machine and remove transitional startup patterns.
- Add priority/criticality-aware scheduling with latency accounting.
- Introduce kernel tracepoints (switch latency, ISR-to-thread delay, jitter metrics).
