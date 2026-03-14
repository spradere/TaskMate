# 🔌 Architecture Note — interface

## Historical developments
The dedicated `interfaces/` layer appeared as TaskMate matured toward portability and explicit dependency rules. Earlier hardware-oriented code was gradually refactored to isolate portable logical contracts (e.g., GPIO logical signals, common pin modes) from concrete HAL implementation details.

## Current implementation
`interfaces/` defines neutral, cross-platform contracts:
- logical GPIO signal identifiers (`gpio_signals.h`),
- generic GPIO modes/pull semantics (`gpio_commons.h`).

These headers are intentionally lightweight and mostly type/enum based. HAL consumes them to implement physical behavior; higher layers consume them through syscalls/services without needing board register details.

## Well-built code and implementation weaknesses
### Strengths
- Correct direction: interfaces stay below HAL and above pure C primitives.
- Keeps semantics stable while allowing target-specific wiring changes.
- Small API surface helps code review and migration.

### Weaknesses (layer leaks / dependency inversion risk)
- Interface scope is currently narrow (mostly GPIO), so many subsystems still depend on HAL headers directly.
- Lack of formal versioning for interface contracts may cause silent breaks when enums evolve.
- Some naming and file comments are slightly inconsistent (`common` vs `commons`), raising friction in larger teams.

## Future improvements for industrial-grade embedded RTOS
- Future improvements should focus on Expand interface contracts to timers, serial channels, clocks, and synchronization primitives
- Add versioned interface IDs and compile-time compatibility guards
- Create architecture compliance tests ensuring HAL implementations respect interface invariants, and Define explicit “no include upward” lint checks to prevent accidental dependency inversion.
