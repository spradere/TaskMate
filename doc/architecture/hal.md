# 🔧 Architecture Note — hal

## Historical developments
TaskMate started as AVR-centric code, then moved to a layered HAL model (notably around v0.21) with architecture (`hal/arch`), MCU (`hal/mcu`), and board (`hal/board`) separation. This progressively removed direct hardware code from `sysCore` and enabled cleaner portability planning (amd64 test path, arm target roadmap).

## Current implementation
HAL remains split by hardware granularity:
- `arch`: context, stack model, low-level startup.
- `mcu`: peripherals (GPIO, timers, USART, I2C).
- `board`: physical wiring and board-specific devices.

Public HAL headers in `hal/public/` present stable include entry points, while selected target files are pulled through generated auto headers. This keeps target selection centralized and avoids ad-hoc includes across upper layers.

## Well-built code and implementation weaknesses
### Strengths
- Clear `arch -> mcu -> board` separation is now materially cleaner after inversion fixes.
- Lower-level register manipulation is isolated in MCU/arch implementations.
- Public façade headers reduce direct include sprawl in non-HAL layers.

### Remaining weaknesses
- Some service code still calls HAL user APIs directly, bypassing stricter syscall mediation.
- Board signal mapping is still imperative and not yet data-driven/generated.
- Capability contracts (mandatory vs optional drivers) are still convention-based.

## Future improvements for industrial-grade embedded RTOS
- Define explicit HAL capability traits with compile-time conformance checks.
- Move board mapping to declarative generated tables with validation.
- Add host-side HAL behavioral tests (mock/fake backend) for non-target regression coverage.
