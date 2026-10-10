# C return and public value type audit

Date: 2026-10-10

Version tag: `v0.32`

HEAD commit: `0f9a403683b7ff1dc28d603a75c2186f47ffa149`

## Purpose and scope

This audit reviews C return types, public API value types, externally linked variables, and repeated internal value domains under `srcs/`, excluding `srcs/autoCode/` and generated implementation fragments.

The review covers the AVR8 reference stack, the FreeBSD host stack, interfaces, HAL, sysCore, sysCall, services, tmLibc, tasks, and target GPIO configuration.

The audit evaluates representation width, integer promotion, AVR atomicity, units, hardware meaning, logical intent, and whether a typedef improves an actual contract rather than merely renaming a local integer.

No source correction is included. Changes to generated contracts must later be made through their source of truth and validated as system-critical autoCode work.

## Verdict

The code generally uses suitable fixed-width integers, `bool`, byte-oriented HAL data, semantic enums, opaque contexts, and bounded text types. Generic C types remain appropriate for local counters, standard-library-compatible returns, promoted variadic arguments, host library values, and values whose field or parameter name already states the unit.

Five domains deserve stronger or more consistent public types: error codes, module identifiers and counts, run levels, encoded status words, and I2C addresses. Two public task variables should instead be removed because they have no public contract or use.

The most important issue is not cosmetic naming. Some representations change size with the target or become unsafe at a declared capacity boundary. A typedef should fix width and express intent where storage, ISR access, generated limits, or multiple layers share the value.

## Decision summary

| Domain | Current representation | Decision |
| --- | --- | --- |
| Error code | Generated enum `err_codes_t` | Replace stored and transported codes with a fixed-width semantic type; keep error constants separately representable. |
| Module identifier | `uint8_t` internally and `uint16_t` in sysCall metadata APIs | Introduce an 8-bit module identifier type and a distinct 16-bit module count type. |
| Run level | Raw `uint8_t` and untyped macros | Introduce an 8-bit run-level type and use it across interfaces, sysCore, sysCall, and control data. |
| Driver and thread status word | `hal_driver_status_t` partly used, otherwise raw `uint8_t` | Use the existing driver status type at public boundaries and add a matching thread status type. |
| I2C address | Raw `uint8_t` in interfaces, records, and scan storage | Introduce an 8-bit I2C address type while retaining `uint8_t` for wire bytes. |
| RTC calendar | `hal_rtc_time_t` with fixed-width fields | Keep as-is; the enclosing structure supplies sufficient meaning. |
| GPIO signal and physical pin | Generated signal enum, port enum, pin structure, `bool` level | Keep the value types; improve error reporting separately if invalid public signal values must be handled. |
| HAL driver result | `hal_driver_state_t` | Keep as-is; it expresses lifecycle state and matches the common driver contract. |
| Atomic token | `hal_atomic_state_t` over `uintptr_t` | Keep as-is unless measured AVR stack pressure justifies a target-selected representation. |
| Stack word and execution context | Target-selected `hal_stack_word_t` and opaque `hal_context_t` | Keep as-is; these are good hardware and structural abstractions. |
| Text comparison and formatting result | `int` | Keep as-is for C comparison semantics and standard-library compatibility. |
| Text and transport byte | `char` above sysCall and `uint8_t` below it | Keep as-is; this matches the documented layer boundary. |

## Findings

### 1. High: the module count domain exceeds the internal loop type

`MOD_COUNT_MAX` is 256 in `srcs/interfaces/tm_modules.h`, while sysCore identifiers, current-thread storage, generated database access, and most iteration variables use `uint8_t` in `srcs/system/sysCore/sys_threads.c`, `srcs/system/sysCore/sys_scheduler.c`, and `srcs/system/sysCall/sc_driver.c`.

The public metadata APIs return counts and accept identifiers as `uint16_t`, then cast validated identifiers to `uint8_t`. This is safe for identifiers 0 through 255, but an 8-bit loop compared with a configured count of 256 wraps to zero and cannot terminate.

The types currently conflate two different ranges: an identifier needs values 0 through 255, while a count needs values 0 through 256. The current `test1` values of 5 drivers and 4 threads do not trigger the fault.

Recommendation: define a fixed 8-bit module identifier type and a fixed 16-bit module count type in the neutral module contract. Use the count type for counts and iteration, and use the identifier type only after the value is proven to identify an existing record. Driver and thread aliases may remain separate if API readability is preferred, although ordinary C typedef aliases do not provide strong type separation.

### 2. High: error-code width and ISR atomicity depend on enum packing

`err_codes_t` is generated as an enum. The AVR build uses `-fshort-enums`, while the FreeBSD build does not. The same logical type can therefore have different widths across targets, and its width can increase when the generated catalogue grows.

`usart_last_error` is a volatile `err_codes_t` shared with the AVR USART receive ISR in `srcs/hal/mcu/atmega2560/at2560_usart.c`. Its comment assumes one-byte storage. That assumption is true for the current 21-value `test1` catalogue, but it is not enforced by the type. A catalogue whose `ERROR_COUNT` enumerator reaches 256 requires a representation wider than 8 bits even though every valid error identifier still fits in one byte.

The variable is read through an atomic syscall path today, but driver-local writes and future uses should not depend on a compiler enum-size option. The same enum also determines the size of the error member in `hal_driver_control_data_t`.

Recommendation: use a fixed-width semantic error-code type for stored and transported valid codes, keep the catalogue count in a wider count type, and add generation-time and compile-time checks that every emitted error identifier fits. Do not use a one-past-the-last enumerator to force the stored code type wider. This change must include the generator source, generated contract, ISR-shared storage, control union, and tests.

### 3. Medium: `err_getMessage()` discards the error-code type

`err_getMessage()` accepts `uint8_t` in `srcs/system/sysCall/sc_errors.h`, so every caller holding an `err_codes_t` casts it before lookup in SCLI and date command code.

The casts hide the catalogue domain, permit unrelated bytes, and would silently truncate if the enum representation grows. They also make the lookup API inconsistent with every syscall that returns `err_codes_t`.

Recommendation: accept the semantic error-code type, validate it against the separately typed catalogue count, and perform any array-index conversion inside the implementation after validation.

### 4. Medium: run levels are a repeated logical domain represented as raw bytes

Run levels cross interfaces, HAL driver control data, sysCore records, scheduler state, sysCall APIs, services, and generated configuration. They are currently raw `uint8_t` values combined with integer macros from `srcs/interfaces/tm_runLevel.h`.

The value is not an arbitrary byte. It has five valid logical values, occupies three encoded status bits, and has ordering semantics. Raw bytes allow unrelated counts, status values, and parsed input to pass through the same signatures without making the conversion point visible.

Recommendation: add `tm_run_level_t` as an explicitly 8-bit type, give constants unsigned fixed-width-compatible values, and return the semantic type from the run-level extraction helper. Continue validating public input because a C typedef does not restrict values.

### 5. Medium: encoded status types are applied inconsistently

The driver layer correctly defines `hal_driver_status_t`, but `sc_driverGetInfo()` exposes its status output as `uint8_t *` in `srcs/system/sysCall/sc_driver.h`. Thread records store their encoded status as a raw `volatile uint8_t`, while only individual bit positions have the semantic `tm_thread_status_bit_t` type.

These values are not general bytes. They combine a run-level field with lifecycle flags and are manipulated by bit macros across layers.

Recommendation: use `hal_driver_status_t *` for the public driver status output and introduce `tm_thread_status_t` for thread status storage. Keep bit-position enums distinct from status-word types.

### 6. Medium: driver lifecycle syscalls lose actionable error results

`sc_driverInit()`, `sc_driverStart()`, and `sc_driverStop()` return `bool`. Their helper receives a semantic `hal_driver_state_t`, but reduces driver error, dead state, invalid name, and dependency failure to false in `srcs/system/sysCall/sc_driver.c`.

This differs from peripheral syscalls such as RTC, LCD, I2C, USART, and console operations, which return `err_codes_t` and support useful diagnostics. A boolean remains suitable for metadata validation and readiness predicates, but it is insufficient when an operation already has a defined failure catalogue.

Recommendation: return the fixed-width semantic error type from driver lifecycle syscalls if callers are expected to diagnose failure. Add a specific name-not-found error and preserve the HAL last error. If the intended contract is only command acceptance, retain `bool` but state that diagnostic loss explicitly in the API documentation.

### 7. Medium: two unused task variables have accidental public linkage

`task1_msg_channel` and `task2_msg_channel` are non-static definitions in `srcs/user/tasks/task1.c` and `srcs/user/tasks/task2.c`. They have no header declaration and no use in the repository.

Their problem is ownership rather than integer width. Giving them a channel typedef would formalize an interface that does not exist and would conflict with the rule that task state remains private unless intentionally exposed.

Recommendation: remove both variables. If a message-channel facility is later introduced, define its identifier type at the owning architectural boundary, expose operations rather than writable global storage, and keep each task's selected channel private.

### 8. Low: I2C addresses are indistinguishable from payload bytes

I2C addresses use `uint8_t` in `drv_i2c.h`, driver records, scan state, and sysCall discovery storage. The same primitive type also represents TWI status values and transferred data bytes.

The representation width is correct, but the domains have different validation and bit-shift rules. The driver address also admits a project sentinel for no address.

Recommendation: define `hal_i2c_address_t` over `uint8_t` in the neutral I2C interface and use it for bus addresses and declared driver addresses. Retain `uint8_t` for transferred bytes and hardware status-register values. Define and validate the no-address sentinel as part of the module metadata contract.

### 9. Low: GPIO read cannot represent an invalid public signal

`hal_gpioSignalRead()` and `sc_gpio_signalGet()` return `bool`, while setters return `void`. Those types are sufficient only if every caller is guaranteed to pass a generated valid `gpio_signal_t`.

A false electrical level is indistinguishable from an invalid or unsupported signal, and the public API provides no result for initialization or write failure. This is an API result-domain limitation rather than a reason to replace `bool` as the representation of a GPIO level.

Recommendation: retain `bool` for the level itself. If public validation is required, return an error code or success flag and deliver the level through an output parameter. Keep this change separate from the core typedef cleanup because it changes syscall behaviour.

## Generic types that should remain generic

- Keep `int main(void)` because it is the hosted and embedded C entry-point contract.
- Keep `int` for string comparison results because the contract is negative, zero, or positive rather than an exact-width quantity.
- Keep `int` for `tm_printf()` and `tm_snprintf()` compatibility, and keep `int` and `unsigned int` when reading promoted variadic arguments.
- Keep ncurses geometry, colour, key, and error values as `int` at the host library boundary.
- Keep `bool` for predicates, readiness, parsed-command success, GPIO electrical values, and host helper success when there is no useful multi-cause error domain.
- Keep `uint8_t` for registers, byte buffers, character codes below the text boundary, and small function-local indices whose bound is explicitly at most 255.
- Keep `uint16_t` for current AVR stack byte measurements and software counter values. Their existing parameter and variable names should state `bytes`, `words`, or `ticks`; a new typedef is not justified until those quantities are shared more broadly or support arithmetic with incompatible units.
- Keep `char` for task, service, and tmLibc text, with explicit checked conversion to `uint8_t` at sysCall and HAL transport boundaries.

## Existing semantic types to preserve

- `hal_driver_control_t`, `hal_driver_status_bit_t`, and `hal_driver_state_t` correctly separate commands, bit positions, and lifecycle results.
- `hal_rtc_time_t` gives sufficient structural meaning to fixed-width calendar fields without seven narrow typedefs.
- `gpio_signal_t`, `gpio_pin_mode_t`, `gpio_pin_pull_t`, `hal_port_list_t`, and `hal_pin_t` already express logical and physical GPIO roles.
- `hal_context_t` is correctly opaque and `hal_stack_word_t` is correctly selected by the target architecture.
- `hal_atomic_state_t` is correctly opaque at call sites. Its `uintptr_t` representation is conservative but portable, and atomic sections are short enough that a narrower AVR representation should require measurement before adding target-selected type machinery.
- `tm_string_t` and `tm_string_storage_t` correctly preserve the RAM versus ROM access contract across AVR and host builds.

## Recommended implementation order

1. Split fixed-width error identifiers from the wider error count, update generation, add size and capacity assertions, and remove lookup casts.
2. Split module identifier and module count types, then convert every count-controlled loop before accepting the declared 256-entry boundary.
3. Introduce the run-level type and propagate it through status records, control data, scheduler, sysCall, and services.
4. Apply status-word types consistently and add the I2C address type.
5. Decide whether driver lifecycle and GPIO APIs require diagnostic error returns, then change those contracts in separately scoped work.
6. Remove the two unused task variables.

Each step should update affected tests, regenerate all changed contracts, inspect generated diffs and logs, build `test1`, and include boundary cases for maximum catalogue and module counts.

## Validation performed during the audit

- Reviewed all tracked production `.c` and `.h` files under `srcs/` except `srcs/autoCode/`, plus the architecture matrix, type rules, and relevant architecture notes.
- Searched all non-autoCode source for typedefs, public function signatures, fixed-width values, plain C integer types, mutable file-scope state, and non-static definitions.
- Confirmed that the current generated `test1` contract contains 5 drivers, 4 threads, and 21 error codes including the count enumerator.
- Ran `bmake TARGET=test1`. Source compilation and ELF linking completed, then the build stopped in a post-link step with `Can't open perl script "perl": No such file or directory`.
- No hardware execution was performed.

## Conclusion

TaskMate does not need pervasive typedefs for every integer. The useful changes are concentrated at repeated architectural boundaries and at values whose width affects capacity or atomicity.

The priority is to make error codes and module count semantics mechanically safe. Run-level, status-word, and I2C address typedefs then improve API intent with negligible AVR cost. Standard C types should remain where they are part of the C ABI, a host library ABI, byte transport, text handling, or simple bounded local arithmetic.
