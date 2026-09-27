# Audit of `hal_*GetStatus()` and `hal_*Control()` factorisation

## Purpose and scope

This audit assesses the factorisation of the six drivers registered for the current
`avr8 / atmega2560 / arduinoMega` target: I2C, USART, scheduling timer, STC timer, AMC2004
LCD, and ZS042 RTC. It covers the private `hal_*GetStatus()` functions and the public
`hal_*Control()` entry points. It does not propose changes to the drivers' functional
operations.

The figures below are static estimates for AVR8 with the current options
(`-Os`, `-fshort-enums`, separate sections, and LTO). They provide orders of magnitude,
not binary measurements, because the AVR toolchain is unavailable in the audit environment.
Any implementation must therefore be compared with the reference binary using `avr-size`,
`avr-nm`, and `avr-objdump`.

## Findings

The six `Control()` functions use the same protocol:

1. three driver-specific life-cycle commands (`INIT`, `START`, and `STOP`);
2. seven generic commands that read or modify the run level, status bits, computed state,
   and last error;
3. the same pointer, level, and bit-index checks;
4. an `ERR_HAL_DRIVER_INVALID_CONTROL` error for every other command.

This represents six copies of a block of about 55 lines, of which about 45 lines are entirely
generic. The six `GetStatus()` functions also share the same local state machine: `DEAD`
takes priority, followed by `ERROR`, `INIT` and `START` consistency, then conversion to a
public state.

The implementations are not completely identical:

- LCD and RTC check the I2C driver state after `DEAD` and `ERROR`, but before the `INIT`
  and `START` bits;
- each driver keeps its state and last error in private objects;
- the USART last error is `volatile` because the RX ISR also writes it;
- `INIT`, `START`, and `STOP` remain hardware-specific and may access volatile registers
  or enable interrupts.

The test priority is part of the observable behaviour. Moving the I2C dependency check before
`DEAD`, for example, can replace a local fatal error with a dependency error. Factorisation
must therefore preserve this order instead of reducing the state machine to a simple table.

## Options considered

### 1. Macro or source generation

A preprocessor template, a parameterised included file, or build-time generation can produce
the current bodies by injecting the names of the state, error, and three life-cycle functions.

| Effect | Estimate |
|---|---:|
| Flash | 0 bytes, apart from a few bytes of possible LTO optimisation |
| Static RAM | 0 bytes |
| Stack | 0 bytes |
| CPU time | 0 cycles |
| Handwritten source | about 250 to 300 lines removed |

This option offers behaviour closest to the existing binary, but moves complexity into the
preprocessor or generator. It makes diagnostics and debugging less direct. Extending autoCode
only for these six drivers would be disproportionate. A generator becomes useful only if the
number of drivers increases substantially or if their declarations already become
configuration data.

### 2. Shared run-time functions without persistent descriptors

A shared HAL unit can provide:

- an evaluator for the local state machine that takes the addresses of the state byte and last
  error;
- processing for the seven generic commands;
- a small wrapper for each driver that handles `INIT`, `START`, and `STOP`, then calls the
  common processing.

LCD and RTC would retain a state wrapper to insert the I2C check at the exact point in the
sequence. Pointers can be passed on each call, avoiding any RAM table.

| Total effect for six drivers | Estimate |
|---|---:|
| Flash | net saving of 350 to 650 bytes |
| Static RAM | 0 bytes |
| Maximum stack | +2 to +8 bytes depending on register allocation and LTO |
| Simple `GETSTATUS` | +8 to +25 cycles |
| Generic command | +12 to +35 cycles |
| Life-cycle command | +8 to +20 cycles |

The ranges include an additional call and return, address loading, and possible stack spills
depending on LTO decisions. They exclude peripheral-specific time. For LCD and RTC, the I2C
query greatly exceeds this overhead. The flash saving is plausible because the large `switch`
exists only once, but LTO may inline the helper again and cancel part of the saving. The helper
must therefore be measured with and without an attribute that prevents inlining. Such an
attribute should not be imposed before the disassembly has been inspected.

### 3. Generic descriptor with callbacks

A descriptor for each driver could contain the addresses of the state and last error, plus
`init/start/stop` callbacks and possibly a dependency callback. This solution further reduces
wrappers and makes adding drivers easier.

On ATmega2560, six descriptors would require about 60 to 84 bytes, depending on the number and
effective size of pointers. In AVR C, an ordinary `const` object does not guarantee zero RAM
use. It may be copied from flash into `.data`. Explicitly placing it in program memory removes
this RAM cost, but requires `pgm_read_*` accesses, complicates portability, and adds cycles.

| Total effect for six drivers | Estimate |
|---|---:|
| Flash, including descriptors | net saving of 400 to 750 bytes |
| Static RAM, ordinary `const` | cost of 60 to 84 bytes |
| Static RAM, explicit program storage | 0 bytes |
| Maximum stack | +4 to +10 bytes |
| Command | +20 to +55 cycles, excluding the hardware operation |

The small additional flash saving does not justify 60 to 84 bytes of RAM on an 8 KiB target,
or a new cross-cutting `PROGMEM` abstraction. Indirect calls also make call-graph and
worst-case-time analysis more difficult. This option is therefore rejected for the current
target.

### 4. Shared `static inline`

A `static inline` helper in a header improves source maintenance but allows a specialised copy
in every translation unit. With `-Os` and LTO, the result may range from good sharing to full
duplication. Neither flash savings nor CPU time are predictable. This option is not retained as
a measurable optimisation strategy.

## Functional and concurrency constraints

- The state byte is atomic on AVR8, but factorisation must preserve the current order of state
  and register accesses. It must not introduce a generic critical section.
- The USART last error must remain `volatile`. A shared API must not discard this qualifier
  through pointer conversion. It needs either a dedicated path or a contract that explicitly
  accepts a volatile pointer.
- `GETLASTERROR` is not a side-effect-free read. The final `GetStatus()` call may replace the
  returned error in internal state if the bits describe an invalid state. Tests must cover this
  behaviour before refactoring.
- `SETBIT` and `CLEARBIT` expose `INIT`, `START`, `ERROR`, and `DEAD`. Every combination,
  including `START=1` with `INIT=0`, must retain the same result and last error.
- The shared code belongs in a neutral HAL unit that `mcu/` and `drivers/` can consume. It
  must not be placed in `interfaces/`, which contains only the portable contract and no
  run-time logic.

## Verdict and recommendation

Factorisation is **technically feasible**. For six drivers, option 2 offers the best balance: a
HAL helper compiled once, no persistent descriptor, and explicit wrappers for life-cycle and
dependency handling. The expected saving is **350 to 650 bytes of flash**, with no additional
static RAM, at a cost of about **8 to 35 cycles** and a few stack bytes per command. This cost is
acceptable for management commands, which are not on the scheduling ISR path. The generic
helper must nevertheless not be used from an ISR.

If the primary objective is only to remove repetition without any change in the binary or WCET,
option 1 is preferable. A small local template is recommended instead of an autoCode extension.

Implementation should be separated into two measurable steps:

1. share and test only the `GetStatus()` state machine;
2. then share the seven generic `Control()` commands.

A reasonable acceptance threshold for retaining the refactor is a measured saving of at least
**256 bytes of flash**, **0 bytes of static RAM**, and a maximum overhead of **40 cycles** for a
generic command. Below this saving, the generated version with no run-time cost is more
appropriate.

## Required validation plan

1. Build a clean reference with `bmake clean && bmake`, and archive the ELF file, map, and
   `avr-size` output.
2. Add table-driven tests for all 16 combinations of the four state bits, with and without an
   available I2C dependency, and verify both the returned state and last error.
3. Build each step with exactly the same options and compare `.text`, `.data`, `.bss`,
   symbols, and disassembly.
4. Count cycles on the `GETSTATUS`, `RLGET`, `SETBIT`, invalid-command, and `INIT` paths,
   including prologues, epilogues, and any indirect calls.
5. Search for calls from ISRs and check stack depth on the worst path.
6. Run `bmake cppcheck` and the boundary validation included in `bmake`.
7. Finally, validate run-level transitions, interrupt-driven USART, timers, and loss of the I2C
   dependency as seen by LCD and RTC on an Arduino Mega.
