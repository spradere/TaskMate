# 🔌 Architecture Note — interface

## Historical developments
The dedicated `interfaces/` layer appeared as TaskMate matured toward portability and explicit dependency rules. Earlier hardware-oriented code was gradually refactored to isolate portable logical contracts (e.g., GPIO logical signals, common pin modes) from concrete HAL implementation details.

## Current implementation
`interfaces/` now acts as the canonical portability contract layer:
- platform-neutral GPIO contracts (`gpio_signals.h`, `gpio_commons.h`),
- shared system-wide constants and compile options (`macros.h`, `options.h`),
- generated global catalogs consumed across layers (`error_catalog.h`).

Rules are now clearer in practice: interfaces define data contracts, HAL implements them, and upper layers consume them through sysCore/sysCall APIs without pulling hardware details into the contract layer.

## Well-built code and implementation weaknesses
### Strengths
- Include direction is cleaner and easier to audit after header split.
- Interfaces remain lightweight and mostly type/constant driven.
- Shared constants no longer require coupling to kernel entry headers.

### Remaining weaknesses
- Coverage is still partial (GPIO + common macros/options + error catalog); other domains remain implicit.
- Contract versioning is still informal; enum or struct evolution can silently break downstream code.

## Future improvements for industrial-grade embedded RTOS
- Extend interfaces to scheduler-facing timing contracts, serial channels, and synchronization primitives.
- Add interface ABI/version markers with compile-time compatibility checks.
- Add CI include-graph checks proving “interfaces depend on nothing above minimal C + interfaces”.
