# Audit: C++ driver instantiation on AVR8

## Purpose and scope

This audit evaluates C++17 templates, `constexpr`, and strong types for statically instantiated TaskMate drivers. The goals are one maintained implementation of the common driver control protocol and one implementation of a peripheral algorithm shared by multiple physical instances. The reference is `test1`, `avr8 / atmega2560 / arduinoMega`. This is a design audit; it changes no firmware, generator, build rule, or historical audit.

Here, zero cost means no dynamic allocation, no added persistent RAM for generic dispatch, and no material regression in flash, stack, or worst-case execution time against an equivalent C implementation. It does not mean that C++ syntax alone guarantees one machine-code copy or zero instructions for every abstraction.

## Current evidence

- `test1_init.rc` declares five drivers: `timerContext`, `usart`, `i2c`, `lcd`, and `rtc`. The scheduling timer, USART, and I2C are MCU implementations; LCD and RTC are reusable drivers. The older `driver_control_factorisation.md` audit describes a previous six-driver snapshot and must not be treated as the current inventory.
- Each current `hal_*Control()` exposes `INIT`, `START`, `STOP`, and seven common state, run-level, bit, and error commands through `hal_driver_control_t` and `hal_driver_control_data_t`. The driver state is private to its implementation file. LCD and RTC also check the I2C driver's state; the USART last error is written by an RX ISR and is `volatile`.
- autoCode emits a named `hal_<name>Control` call to set the run level and stores that function in each generated `mod_driver_item_t`. `sysCall` uses these runtime function pointers for driver enumeration, name lookup, run-level startup, and SCLI control. Compile-time driver types alone cannot remove that runtime dispatch without changing these public behaviors.
- The current `mod_driver_item_t` stores an address and a control function pointer for each driver in static RAM. A C++ design should first reuse this record rather than add a second per-instance descriptor or a virtual table.
- `mk/sources.mk` discovers only `.c` files, maps only `.c` to objects, and the AVR compile and link recipes invoke `avr-gcc`. There is no C++ linkage convention in the project headers. Any firmware `.cpp` requires coordinated build and interface work.
- The installed `avr-g++` is GCC 14.2.0. A standalone C++17 `constexpr` template probe compiled and linked for `atmega2560` with `-Os`, LTO, and disabled exceptions/RTTI. This proves basic compiler availability, not that the full firmware or its headers compile as C++.
- The installed driver reports `libstdc++.a` and `libsupc++.a` as unresolved names, so this environment does not expose those libraries through the compiler search path. A prototype should use a small freestanding C++ subset and check every linked runtime symbol.

## Feasibility and limits

| Design | Source reuse | Code sharing across instances | Main cost or constraint |
| --- | --- | --- | --- |
| Templated control with state and hooks as template parameters | High | Uncertain; each distinct specialization can emit a separate body | Status addresses, hooks, and inlining can prevent folding |
| One non-template control engine taking an instance state reference | High | One body by construction when compiled out of line | Arguments and calls can add cycles and stack use |
| Compile-time pin or register policy with inline operations | High | Usually one specialization per hardware mapping | Fast direct register access can increase flash |
| One runtime peripheral engine with an instance context | High | One algorithm body by construction | Register selection and context access cost cycles |
| Virtual driver base or callback-rich descriptor | High | Shared method bodies possible | Vtables, indirect calls, and possible RAM or flash data |

`constexpr` can validate addresses, widths, pin mappings, and legal configuration at compile time. It creates no per-instance object by itself. Strong types can prevent mixing bus numbers, addresses, pins, and run levels inside C++ HAL code. Neither feature removes the runtime driver catalogue, its function pointers, or the state bytes required by each live instance.

GCC can fold semantically identical functions under `-Os` and can optimize across files with LTO, but specialization does not guarantee identical machine code. `extern template` and one explicit instantiation avoid repeated emission of the same specialization across translation units; they do not merge different register addresses or policy types. A template-only design therefore cannot guarantee both direct constant-address instructions and one shared implementation for distinct peripherals.

For multiple instances of one base peripheral, the practical design is a shared non-template transaction or control engine operating on an explicit instance context. Thin typed wrappers select and validate the instance at compile time. An AVR register interface can be passed as a compact base address or a small operation set, but the measured extra address loads or indirect calls decide whether it meets the time budget. A specialized fast path remains appropriate for a measured ISR or timing-critical operation; it should not be forced into the shared engine merely to eliminate source duplication.

The current firmware is C. C++ functions called by C files require a C ABI boundary. The project rule formally prohibits `extern`, which includes the usual `extern "C"` spelling. Thus a mixed C/C++ implementation cannot be adopted under the present rule without an explicit policy change. Converting the entire firmware to C++ would avoid project-internal language linkage at the cost of a much broader port, including generated C fragments and C-specific constructs. The mixed-language option is narrower, but its C ABI declarations require a separately approved exception to the rule. Avoid assembler symbol aliases or indirect include tricks to bypass it.

## Proposed architecture if C++ is authorized

1. Keep `interfaces/` as a C-compatible, dependency-neutral contract. Keep `sysCore`, `sysCall`, and autoCode in C for the first prototype. Place C++ templates and strong HAL-only types under `srcs/hal/`, never in the neutral interface headers.
2. Define one private instance context per physical peripheral. It owns status, last error, and hardware parameters. Use static storage and constant initialization. Keep interrupt-shared fields qualified and preserve their existing atomicity rules. Do not use dynamic allocation or local statics that require runtime guards.
3. Implement the seven common commands and status calculation in one out-of-line, non-template HAL core. Pass the context explicitly. Keep lifecycle operations and dependency checks as small driver-specific wrappers so their state transition order and error side effects remain unchanged.
4. Use a template policy or a strongly typed `constexpr` configuration only for register and pin selection and compile-time validation. Instantiate each physical configuration once. Route large common operations through the shared core. Keep instance-specific wrappers small enough to inspect in AVR disassembly.
5. Preserve the existing C-visible `hal_<name>Control()` entry points and generated module table during the first migration. For a later multi-instance configuration, extend `init.rc` and autoCode with separate logical instance names and a shared implementation source; generate distinct wrappers and contexts, not copies of the base driver source. Confirm that source discovery compiles that source once.
6. Change the build only after the interface policy is settled: discover selected `.cpp` files, map them to distinct objects and dependencies, compile them with `avr-g++`, and link with the C++ driver. Apply C++ flags only to C++ sources; the existing C warning flags are not all valid for C++. Keep boundary checks, generated include paths, LTO, section garbage collection, map output, and target isolation working.

## Ordered implementation and acceptance plan

1. Record a clean `test1` C baseline: ELF, map, `avr-size -A`, `avr-nm -S --size-sort`, control symbols, and worst-case stack path. Keep identical compiler versions and flags for every comparison.
2. Resolve the `extern` policy explicitly before adding a mixed-language C ABI. If the policy remains unchanged, stop the mixed-language prototype and evaluate a C shared core or an explicitly scoped full C++ migration instead.
3. Prototype one ordinary driver control path and two instances of one simple peripheral in an isolated build. Prove that the common control and peripheral algorithm each have one out-of-line body in the linked ELF. Inspect wrappers and symbols; source-level reuse is insufficient evidence.
4. Test every command, invalid pointer/value, all `INIT`/`START`/`ERROR`/`DEAD` combinations, run-level mutation, and last-error side effects against the C baseline. Cover the LCD/RTC I2C dependency and the ISR-visible USART error separately before migrating them.
5. Compare flash, `.data`, `.bss`, stack depth, and disassembly for one and two instances. Attribute any added bytes to contexts, wrappers, descriptors, C++ runtime, and register-access strategy. Check direct and indirect call cycles on the control path; check interrupt latency if any ISR calls the code.
6. Run the affected autoCode tests, boundary guards, and `bmake TARGET=test1`; validate the board's USART, timer, I2C, LCD, and RTC behavior. A successful build alone cannot validate AVR interrupt and stack behavior.

Acceptance requires one linked common control body and one linked shared body for the selected multi-instance operation, no added generic dispatch state in static RAM, no unexpected C++ runtime dependency, unchanged observable control behavior, and measured flash, stack, and cycle results acceptable for the target. If the common engine exceeds the accepted timing budget, keep a measured specialized fast path and document that tradeoff; do not claim unconditional zero cost.

## Verdict

The approach is feasible as a constrained hybrid: C++ supplies compile-time configuration and type checks; one explicit runtime core supplies guaranteed code sharing; small wrappers preserve the existing module API. A pure per-instance template design meets source-reuse goals but cannot guarantee one binary copy for distinct peripherals. The immediate blockers are the build's C-only source pipeline and the project's `extern` prohibition for a mixed-language C ABI. No production flash or cycle saving is claimed until an AVR ELF comparison has been made.

## References

- Local contracts and composition: `srcs/interfaces/hal_drivers.h`, `srcs/system/sysCore/sys_drivers.h`, `srcs/autoCode/tagWriters/tagWriters_drivers.c`, `srcs/user/target/test1/test1_init.rc`, `mk/sources.mk`, and `srcs/hal/arch/avr8/avr8_CC.mk`.
- GCC documentation: [template instantiation](https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Template-Instantiation.html), [optimization and identical code folding](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html), and [C++ dialect options](https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Dialect-Options.html).
