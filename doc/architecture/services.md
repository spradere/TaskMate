# 🧩 Architecture Note — services

## Historical developments
Services introduced reusable system threads above the kernel, including the serial CLI.
After `v0.28`, they moved under `srcs/system/services/` and dropped their direct HAL access.
Commit `5109e98` routed SCLI USART input through sysCall.
Commits `c843372` and `35f329d` established staged startup in the system service.
Run-level admission and cooperative yield then made startup ordering explicit.
At `v0.31`, `test1` remains the sole composition and always includes system and SCLI services.

## Current implementation
autoCode registers `system` at the core level and `scli` at the service level, each with a fixed
stack. Both declare initialization through sysCall and use sysCall for thread or hardware mediation.

The system service initializes logical GPIO, advances run levels, starts matching drivers, performs
I2C discovery, captures RTC startup time, and waits for driver and thread readiness. It then logs
system data and periodically displays RTC time on the LCD.

SCLI reads USART into a fixed buffer and dispatches `date`, `driver`, `i2c`, and `thread` commands.
Services use tmLibc for strings, formatting, and logging and may consume neutral data contracts;
they do not include concrete HAL headers or call HAL functions directly.

## Well-built code and implementation weaknesses
### Strengths
- Service records, stacks, command tables, and input buffers have fixed memory costs.
- Startup follows explicit core, driver, service, and user stages with bounded readiness rounds.
- Hardware operations and thread control remain behind focused syscalls.
- Generated registration keeps service composition aligned with selected source declarations.

### Remaining weaknesses
- Startup discards scan, RTC snapshot, and individual driver-start results.
- Readiness is self-declared and has no richer dependency, health, or recovery state.
- The display loop ignores RTC and LCD failures after startup.
- SCLI processes available chunks, has no persistent line assembly, and truncates excess arguments.
