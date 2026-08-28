# 💡 Architecture Note — gpio

## Historical developments
GPIO support began as direct MCU pin handling, then was refactored (notably in v0.22 and v0.26) toward separation between logical signals and physical pins. This shifted application code from “port/pin thinking” to named signal semantics.

After v0.28, board-independent target configuration became more explicit. The logical signal list moved
to `user/target/test1/signals.gpio`, autoCode began generating the corresponding enum, and physical
wiring moved from board startup code to `user/target/test1/targetInit.c`. Register manipulation in the
ATmega2560 driver was also consolidated around the shared bit macros.

## Current implementation
The selected target's `signals.gpio` file generates `gpio_signal_t` in
`interfaces/gpio_signals.h`. During boot, `sysCore/gpio.c` iterates over every signal, asks
`targetWireSignal()` to fill a static `hal_signal_t` table, and passes each physical pin to
`hal_gpioPinInit()`.

Tasks call `sc_gpio_signalSet()`, `sc_gpio_signalGet()`, or `sc_gpio_signalToggle()`. These thin syscall
wrappers delegate to sysCore, which indexes the signal table and calls the HAL public GPIO API. The
ATmega2560 implementation maps `PORT_A` and `PORT_B` to DDR, output, and input registers and currently
implements input, push-pull output, pull-up, read, and write operations. The `test1` target maps the
on-board LED and the two example task LEDs.

## Well-built code and implementation weaknesses
### Strengths
- Tasks use generated logical identifiers and never manipulate AVR registers directly.
- Target wiring, logical state, HAL selection, and register access have distinct source locations.
- Tables are statically allocated, and the normal read/write path has fixed execution cost.
- Unsupported target/MCU combinations fail through the HAL public header selection.

### Remaining weaknesses
- `active_high` is written by the target but never applied by sysCore, so logical reads, writes, and
  toggles currently expose physical polarity.
- Signal, table, port, and pin inputs are not checked. `targetWireSignal()` has no result value, and
  its defaults can silently turn a missing mapping into a valid-looking pin configuration.
- Interface modes include open-drain, high-impedance, and pull-down, but the ATmega2560
  implementation handles only input, push-pull output, and pull-up; only ports A and B are
  described.
- Toggle is implemented as an unprotected read followed by a write. Wiring also lacks generated
  completeness/duplicate-pin validation, debounce, edge delivery, and an explicit ISR/concurrency
  contract.
