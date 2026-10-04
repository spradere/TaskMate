# Firmware flash size reduction audit

## Purpose and scope

This audit investigates significant firmware flash reductions for the current `test1 / arduinoMega / atmega2560 / avr8` target. It covers compilation options, services, diagnostics, the error catalogue, the formatter, and the common driver protocol.

The analysis corresponds to commit `400ac8b`. This audit includes no firmware behaviour change.

## Method

The reference was rebuilt with:

```sh
bmake clean && bmake
avr-size -G -d build/test1_arduinoMega_atmega2560_avr8/TaskMate.elf
avr-nm --format=posix --print-size --size-sort -r \
	build/test1_arduinoMega_atmega2560_avr8/TaskMate.elf
```

The flash measurement is `.text + .data`. The `.data` section occupies RAM at run time, but its initial image is also stored in flash.

Compilation variants were rebuilt completely with the same sources. Functional variants were linked with minimal substitutes so that LTO and `--gc-sections` could remove paths that had become unreachable. These links measure feature contributions, but they are not validated, executable firmware images.

## Reference

The clean build ends with `Build complete` and produces:

| Section | Size |
|---|---:|
| `.text` | 15,678 bytes |
| `.data` | 454 bytes |
| Total flash | 16,132 bytes |
| `.bss` | 1,364 bytes |

The main code symbols are:

| Symbol | Size |
|---|---:|
| `main` | 984 bytes |
| `system` | 642 bytes |
| `scli` | 542 bytes |
| `tm_vsnprintf` | 538 bytes |
| `sc_i2cScan` | 528 bytes |
| `hal_lcdControl` | 404 bytes |
| `sc_rtcRead` | 372 bytes |

The project already enables `-Os`, `-mrelax`, `-fshort-enums`, LTO, separate sections, and `--gc-sections`. Simply enabling LTO or section garbage collection is therefore not a new option.

## Measured results

| Variant | Flash | Saving | Measurement type |
|---|---:|---:|---|
| Reference | 16,132 bytes | - | complete build |
| `-mcall-prologues` | 15,480 bytes | 652 bytes, 4.04% | complete build |
| Two AVR options | 15,398 bytes | 734 bytes, 4.55% | complete build |
| Error catalogue text absent | 15,196 bytes | 936 bytes, 5.80% | experimental link |
| Logs removed | 11,150 bytes | 4,982 bytes, 30.88% | experimental link |
| SCLI chain removed | 8,990 bytes | 7,142 bytes, 44.27% | experimental link |

`-Oz` does not change the result with this AVR-GCC 14.2.0. `-maccumulate-args` and `-fno-jump-tables` increase the size and must be rejected.

The savings are not additive. LTO transforms and combines the whole program after each variation.

## Priority 1: development and production profiles

The largest reduction comes from excluding diagnostic features from firmware that does not use them. Two profiles are recommended:

- `development`: SCLI, commands, complete text, and current logs;
- `production`: no SCLI, ordinary logs removed at compile time, with numeric codes and the `panic()` path retained.

SCLI is registered in `target*_init.rc`. The profile must select the appropriate autoCode input so that the thread database, generated includes, and call graph remain consistent. Excluding only `scli.c` from the Makefile would leave an invalid generated reference and bypass the source of truth.

The experimental removal of the entire SCLI chain saves 7,142 bytes. It removes the commands, their text, their tables, syscalls used only for diagnostics, and the textual catalogue that is then no longer referenced.

The marginal contribution of each command family was also measured:

| Command absent | Flash saving |
|---|---:|
| `date` | 1,970 bytes |
| `driver` | 1,650 bytes |
| `thread` | 1,472 bytes |
| `i2c` | 598 bytes |

These values include everything LTO can remove with the command. Their sum differs from the complete SCLI removal because of shared functions, its USART reader, tokenizer, and dispatch.

If SCLI must remain available, an intermediate profile can select only the required commands. The `date` command, followed by `driver` and `thread`, should be removed first from a compact image.

## Priority 2: AVR compilation options

Adding `-mcall-prologues` to the architecture options saves 652 bytes without changing `.data` or `.bss`. AVR-GCC replaces some repeated prologues and epilogues with shared routines.

Adding `-fno-inline-functions-called-once` increases the saving to 734 bytes. This option materialises more functions and reduces several large bodies produced by LTO, especially `main`, `system`, and `tm_vsnprintf`.

The first flag is the least intrusive change. The second should be accepted only after measuring cycles and stack depth on sensitive paths. Naked ISRs and context switching must be checked in the disassembly even though the compiler should not add an ordinary prologue to them.

## Priority 3: log policy

Literals created by `TM_STR()` account for 1,600 bytes of flash in the reference binary, excluding messages from the error catalogue. The AVR HAL already places the text correctly in program memory. Moving it to `PROGMEM` again would not reduce flash use.

A no-op substitute for `tm_syslog()` allows LTO to remove 4,982 bytes. The saving exceeds the size of the strings because calls, argument preparation, error queries, and several display branches also disappear.

A real policy must use level macros that remove the entire expression in the preprocessor. Calling an empty function without LTO, or continuing to evaluate its arguments, does not guarantee this result. Drivers must continue to expose their states and errors instead of producing messages directly.

The production profile may retain minimal, bounded output for critical startup and `panic()`. It should be separate from ordinary logs and must not depend on SCLI.

## Priority 4: compact error catalogue

The current catalogue directly accounts for:

| Item | Flash size |
|---|---:|
| 18 messages | 517 bytes |
| 18 `tm_string_t` descriptors | 54 bytes |
| 23-entry table | 69 bytes |
| Total identified data | 640 bytes |

The experimental link without text saves 936 bytes after unused functions and branches are removed. The `err_codes_t` values must remain stable. Only their human-readable representation should become optional.

This change belongs in the `*.err` files and autoCode. Generated regions in `srcs/system/sysCall/sc_errors.c` and `srcs/interfaces/error_catalog.h` must not be edited manually. The placeholder `ERR_UNKNOWN` and `ERR_RUNTIME` messages are the first candidates for removal independently of the profile.

## Priority 5: driver controllers

The six `hal_*Control()` functions occupy 1,752 bytes in total:

| Controller | Size |
|---|---:|
| LCD | 404 bytes |
| STC timer | 312 bytes |
| I2C | 304 bytes |
| RTC | 282 bytes |
| scheduling timer | 234 bytes |
| USART | 216 bytes |

They repeat the `RLSET`, `RLGET`, `SETBIT`, `CLEARBIT`, `GETBIT`, `GETSTATUS`, and `GETLASTERROR` processing. The `doc/audits/driver_control_factorisation.md` audit already recommends a HAL helper compiled once, without persistent descriptors. Its estimated saving of 350 to 650 bytes remains plausible in light of the 1,752 bytes now measured.

This factorisation must preserve error ordering, the LCD and RTC I2C dependency, and the `volatile` nature of the USART last error. A binary prototype is essential because indirect callbacks or LTO re-inlining may reduce the benefit.

## Priority 6: compact formatter

`tm_vsnprintf()` occupies 538 bytes. The firmware uses `%i`, `%s`, `%x`, and zero padding, but no current call uses `%c`, `%b`, or `%%`. The generic conversion nevertheless retains all three bases and performs 16-bit division and modulo.

A formatter profile limited to the conversions actually required can remove dead branches that the compiler cannot infer from a string interpreted at run time. The expected saving is secondary to SCLI and logs. It must be measured before reducing the formatter's public contract.

## Unproductive options

- Moving `const` command tables into program memory would mainly save RAM. Their bytes remain in the flash image and AVR accesses become more expensive.
- Removing DWARF sections from the ELF file reduces the debug file size, not the programmed image size.
- Removing the startup I2C scan would change driver discovery and reconciliation. This is not a neutral optimisation.
- Reducing thread stacks does not reduce flash because they reside in `.bss`.
- Replacing the TaskMate formatter with standard `printf` could greatly increase size and weaken the bounds designed for the microcontroller.

## Recommended implementation order

1. Add and measure `-mcall-prologues` in the AVR backend.
2. Introduce a production profile controlled by autoCode inputs, without SCLI.
3. Add compile-time-eliminated log levels and a catalogue without text.
4. Decide command by command what belongs in an optional compact SCLI profile.
5. Prototype controller factorisation and retain it only if the saving is measured.
6. Specialise the formatter last.

After each step, run `bmake autoCode_alone` if its inputs change, then `bmake clean && bmake`. Compare `.text`, `.data`, `.bss`, the map file, and symbols from the same ELF file. Changes to prologues, factorisation, and formatting must also be checked in the disassembly. Finally, the AVR build validates software integration, but not behaviour on an Arduino Mega, interrupts, hardware timing, or the real I2C bus.

## Verdict

The best identified saving without feature removal is 652 bytes with `-mcall-prologues`, or 734 bytes with a more restrictive inlining policy.

A production profile is required for a truly significant reduction. Removing SCLI allows LTO to eliminate about 7.1 KiB, or 44% of the current image. Logs and error text should then be treated as configurable diagnostic features while retaining codes, states, `panic()`, and the `services -> sysCall -> HAL` boundaries.
