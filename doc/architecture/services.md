# 🧩 Architecture Note — services

## Historical developments
Services were introduced after early core scheduling work to provide reusable system-level threads (message server, serial CLI) without mixing application code and kernel internals. Over time, services became autoCode-managed modules with run-level metadata and generated allocation.

## Current implementation
Current services include:
- `msg`: lightweight channel-based message dispatch to USART/LCD.
- `scli`: serial echo/command entry skeleton using `msg` for output.

Services run as scheduled threads, use `sysCall` wrappers for delays/yielding, and call HAL via generated user include exposure. Message channels are static, bounded, and cooperative with periodic sleep via software time counters.

## Well-built code and implementation weaknesses
### Strengths
- Good modularity: services isolated in dedicated directory and init descriptors.
- Static allocation avoids heap unpredictability.
- Threaded service model matches RTOS extension pattern.

### Weaknesses (layer leaks / dependency inversion risk)
- Services can directly include HAL user APIs, bypassing stricter syscall boundaries.
- `msg` currently mixes transport policy and formatting/workflow logic; destination handling is tightly embedded.
- Concurrency robustness is partial (commented TODO around send flag overwrite).
- Busy-wait patterns persist in thread loops and may hurt deterministic CPU budgeting.

## Future improvements for industrial-grade embedded RTOS
- Future improvements should focus on Enforce service-to-HAL access via sysCall facades only (except audited exceptions)
- Add lock-free or bounded-critical-section message queues with explicit ownership
- Introduce service health monitoring (deadline miss counters, queue saturation metrics), and Separate protocol parsing from transport output to make services certifiable and testable.
