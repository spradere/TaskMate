# 💡 Architecture Note — gpio

## Historical developments
GPIO support began as direct MCU pin handling, then was refactored (notably in v0.22 and v0.26) toward separation between logical signals and physical pins. This shifted application code from “port/pin thinking” to named signal semantics.

## Current implementation
GPIO flow is now clearly layered:
1. `interfaces/gpio_signals.h` defines logical signal IDs.
2. `sysCall/sc_gpio.*` exposes task/service-facing logical APIs.
3. `sysCore/gpio.*` owns the signal table and validation path.
4. HAL MCU/board code handles register-level behavior and board wiring.

Boot wiring initializes signal mappings once, then upper layers use only logical IDs.

## Well-built code and implementation weaknesses
### Strengths
- Logical/physical separation is materially clearer.
- Portability improves because tasks avoid raw port semantics.
- GPIO control path is static and deterministic.

### Remaining weaknesses
- Polarity handling coverage should be re-verified for all read/write/toggle call paths.
- Mapping is still imperative; scaling to many signals is harder to audit.
- No built-in debouncing/event queue model yet.

## Future improvements for industrial-grade embedded RTOS
- Make polarity behavior explicitly tested per API path.
- Generate mapping tables from declarative board config.
- Add deterministic edge/event APIs with bounded ISR work.
