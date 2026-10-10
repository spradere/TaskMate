# Architecture Note: services

## Historical developments
Services introduced reusable system threads above the kernel, including the serial CLI.
After `v0.28`, they moved under `srcs/system/services/` and dropped direct HAL access.
Tag `v0.31` marks syscall-only USART input and staged run-level startup.
After `v0.32`, generated driver-capability flags made the system service usable without optional peripherals.
After `v0.32`, SCLI gained generated source declarations, driver and thread lifecycle commands, and stack-depth reporting.
After `v0.32`, service parsing removed pointer arithmetic and low-level byte paths adopted explicit-width types.

## Current implementation
autoCode registers `system` at core level and optionally `scli` at service level with fixed target-configured stacks. Both declare readiness and use syscalls for thread cooperation and hardware mediation.

The system service initializes GPIO, advances run levels, starts drivers, checks driver and thread readiness, and conditionally scans I2C or captures RTC startup time. It logs system data and updates the LCD when those drivers exist.

SCLI polls USART into a fixed 64-byte buffer and dispatches generated `date`, `driver`, `i2c`, `stack`, and `thread` command sets selected by the target. Services may use tmLibc and neutral types, but do not call HAL directly.

## Well-built code and implementation weaknesses
### Strengths
- Service records, stacks, command tables, and buffers have fixed target-known memory costs.
- Startup follows explicit core, driver, service, and user stages with bounded readiness rounds.
- Optional driver capabilities remove unavailable peripheral paths at compile time.
- Hardware and thread lifecycle operations remain behind focused syscalls.

### Remaining weaknesses
- Startup discards I2C scan, RTC snapshot, and individual driver-start errors.
- Readiness is self-declared and has no dependency graph, degraded state, retry, or recovery policy.
- The display loop ignores RTC, formatting, and LCD errors after startup.
- SCLI processes each available RX chunk as a command, does not assemble newline-delimited input, and silently rejects excess arguments.
