# 💡 Architecture Note — gpio

## Historical developments
GPIO began as direct MCU pin access; `v0.22` and `v0.26` separated logical signals from pins.
After `v0.28`, target configuration owned wiring and autoCode generated signal identifiers.
The pre-`v0.31` refactor removed GPIO state and dispatch from sysCore.
Commit `2c3b727` moved logical operations to sysCall and the neutral hardware contract.
Commits `93fc092` to `0eefe86` completed generated wiring and system-only initialization.

## Current implementation
`test1/signals.gpio` defines the logical signal set. autoCode generates its enum and inserts target
wiring into the selected ATmega2560 GPIO translation unit, where the static signal table lives.

The system service alone may initialize signals, enforced by the current-thread check in sysCall.
Initialization asks target configuration to map each signal, then configures the selected MCU pin.

Tasks call set, get, or toggle syscalls. These call the neutral GPIO contract directly; the selected
MCU resolves the table and accesses registers for input, push-pull output, pull-up, read, and write.
sysCore no longer owns logical GPIO state or participates in the normal operation path.

## Well-built code and implementation weaknesses
### Strengths
- Tasks depend on generated logical names, never AVR ports, pins, or registers.
- Target wiring, neutral operations, MCU pin mechanics, and task APIs have distinct ownership.
- Static generated tables keep memory and normal-path timing deterministic.
- Build selection and generation require both a signal list and a target wiring source.

### Remaining weaknesses
- Stored active polarity is not applied, so logical operations still expose physical polarity.
- Signal, port, pin, and table accesses have no runtime bounds validation.
- The portable mode set is wider than the implemented ATmega2560 subset.
- Toggle is a non-atomic read-modify-write and wiring has no duplicate-pin completeness check.
