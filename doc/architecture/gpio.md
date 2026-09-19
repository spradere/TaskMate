# 💡 Architecture Note — gpio

## Historical developments
GPIO began as direct MCU pin handling, then `v0.22` and `v0.26` separated logical signals from pins.
This moved application code from port/pin choices to named signal semantics.

After tag `v0.28`, target configuration took ownership of wiring and autoCode generated signal IDs.
Commit `b201809` completed the HAL/GPIO refactor; ATmega2560 bit operations then converged on shared
helpers.

## Current implementation
The selected target's `signals.gpio` generates the logical signal enum. autoCode also includes the
selected target wiring in the MCU GPIO implementation. Before scheduling starts, system startup
asks the HAL to populate its static signal table and initialize each pin.

Tasks use set, get, and toggle syscalls. These delegate signal resolution to sysCore, which calls
the neutral GPIO contract implemented by the selected MCU. That MCU owns the logical-to-physical
table and supports input, push-pull output, pull-up, read, and write for configured ports.

## Well-built code and implementation weaknesses
### Strengths
- Tasks use generated logical signals and never manipulate AVR registers directly.
- Target wiring, logical state, HAL selection, and register access remain separate.
- Static tables give deterministic memory use and fixed normal-path execution cost.
- Unsupported target or MCU selections fail through build-time target and source selection.

### Remaining weaknesses
- Configured polarity is stored but not applied, so logical operations expose physical polarity.
- Signal, table, port, and pin inputs lack validation; missing wiring can appear valid.
- Interface modes exceed those implemented by ATmega2560, and only a subset of ports is described.
- Toggle is not atomic, and wiring lacks completeness, duplicate-pin, and ISR contract checks.
