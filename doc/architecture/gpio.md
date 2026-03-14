# 💡 Architecture Note — gpio

## Historical developments
GPIO support began as direct MCU pin handling, then was refactored (notably in v0.22 and v0.26) toward separation between logical signals and physical pins. This shifted application code from “port/pin thinking” to named signal semantics.

## Current implementation
The GPIO stack is layered:
1. `interfaces/gpio_signals.h` defines logical signals.
2. `sysCall/gpio.*` exposes signal-level API (`init/set/get/toggle`).
3. MCU HAL (`hal_gpio.*`) performs register-level pin config/read/write.
4. Board HAL (`hal_boardWireSignal`) maps logical signals to physical pins and default mode/pull/active polarity.

Initialization wires all signals at boot (`gpio_signalsInit`) and then tasks manipulate only logical IDs.

## Well-built code and implementation weaknesses
### Strengths
- Good conceptual split: logical signal namespace separated from electrical mapping.
- Board-specific wiring keeps application and services portable across boards.
- Small API with deterministic static behavior.

### Weaknesses (layer leaks / dependency inversion risk)
- `active_high` is stored but not applied in read/write path, so abstraction is incomplete.
- Syscall API is very thin and does not validate ownership, mode transitions, or safety states.
- Signal wiring currently relies on imperative conditionals in board code; harder to scale and audit.
- No explicit debouncing, interrupt-driven GPIO events, or atomic multi-signal operations.

## Future improvements for industrial-grade embedded RTOS
Future improvements should focus on Enforce polarity handling (`active_high`) consistently in syscall/HAL paths; Move mapping to generated static tables with compile-time validation; Add GPIO capability profiles (output-only, safety-critical, interrupt-capable); Provide deterministic event API (edge callbacks/queues) with bounded ISR work, and Add configuration and runtime diagnostics (stuck pin detection, illegal mode checks).
