# 🔧 Architecture Note — hal

## Historical developments
TaskMate started as AVR-centric code, then moved to a layered HAL model (notably around v0.21) with architecture (`hal/arch`), MCU (`hal/mcu`), and board (`hal/board`) separation. This progressively removed direct hardware code from `sysCore` and enabled cleaner portability planning (amd64 test path, arm target roadmap).

## Current implementation
HAL is split by hardware granularity:
- `arch`: context switch, stack model, low-level CPU startup.
- `mcu`: timers, GPIO registers, USART, I2C.
- `board`: pin mapping and board-specific peripherals.

Compile-time generated headers (`auto_hal_*`) expose selected APIs to system/user code. Runtime code uses static function calls, no dynamic probing. GPIO wiring is a good example: logical signal enum is mapped by board code, then configured by MCU GPIO driver.

## Well-built code and implementation weaknesses
### Strengths
- Correct architectural decomposition (arch/mcu/board) for portability.
- Low overhead static binding appropriate for small MCUs.
- Good encapsulation of register-level operations inside MCU files.
- Generated include headers keep feature surface explicit per target.

### Weaknesses (layer leaks / dependency inversion risk)
- Some high-level services include broad HAL user headers, increasing accidental cross-driver coupling.
- HAL fallback and auto-generated includes can blur API ownership (what is mandatory vs optional).
- Board GPIO mapping uses imperative if-chain; scale and maintainability may degrade with many signals.
- HAL still exposes details that can leak timing assumptions upward (scheduler and timer coupling).

## Future improvements for industrial-grade embedded RTOS
- Future improvements should focus on Define strict HAL capability interfaces (timer, gpio, uart traits) with compile-time conformance checks
- Introduce per-driver latency budgets and ISR execution contracts
- Move board signal mapping to declarative tables generated from configuration
- Add hardware abstraction tests (mock HAL on host) to validate behavior without target silicon, and Separate “critical RT” HAL APIs from “best effort” APIs to protect determinism.
