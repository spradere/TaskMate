# Architecture Note: gpio

## Historical developments
GPIO began as direct MCU pin access; `v0.22` and `v0.26` separated logical signals from pins.
After `v0.28`, target configuration owned wiring and autoCode generated signal identifiers.
Tag `v0.31` marks removal of GPIO state from sysCore and the neutral sysCall-to-HAL path.
After `v0.32`, target signal files gained target-qualified names and moved fully into target configuration.
After `v0.32`, the FreeBSD PC target gained virtual GPIO display and syscall toggle became atomic.

## Current implementation
Each target provides a named `*_signals.gpio` list and a `*_signals.c` wiring implementation. autoCode generates the logical enum and includes the selected wiring in the AVR MCU or PC virtual GPIO translation unit.

Only the system thread may initialize logical signals. On ATmega2560, initialization maps every signal to a static pin record and configures registers. On the PC target, it initializes static values and console display names.

Tasks use syscalls to set, get, or atomically toggle logical signals. The selected neutral HAL implementation resolves the generated signal identifier without exposing target pins or registers to tasks.

## Well-built code and implementation weaknesses
### Strengths
- Tasks depend on generated logical names, never physical pins, registers, or host display details.
- Target wiring, neutral operations, MCU pin mechanics, and task APIs have distinct ownership.
- Static tables keep memory and normal-path dispatch bounded and deterministic.
- The build requires a selected signal list and wiring source, and generation checks signal names.

### Remaining weaknesses
- ATmega2560 stores active polarity but does not apply it, so logical values still expose electrical polarity.
- ATmega2560 signal, port, and pin indices have no runtime bounds validation.
- The portable mode and pull enums are wider than the ATmega2560 implementation.
- Generation does not detect duplicate physical pins or prove that target wiring returns a valid mapping.
