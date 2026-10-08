# Audit: C++ control factorisation

## Purpose and scope

This audit evaluates a mixed C and C++ implementation for factorising TaskMate driver control and for instantiating several physical peripherals from one maintained implementation. It covers the C ABI boundary, the move from the previously considered C++17 profile to C++20, `constexpr`, strong types, templates, concepts, RAII, and MISRA C++ application. The reference target is `test1`, `avr8 / atmega2560 / arduinoMega`. This is a design audit. It changes no firmware, generator, build rule, or other historical audit.

Zero cost means no dynamic allocation, no added persistent RAM for generic dispatch, no unexpected C++ runtime dependency, and no material regression in flash, stack, interrupt latency, or worst-case execution time against equivalent C. Source abstraction alone does not prove zero cost. The linked AVR ELF and its disassembly are the evidence.

## Decisions

- `extern "C"` is authorized for declarations and definitions that form an intentional C ABI boundary. This is a narrow exception to the project prohibition on `extern`; it does not authorize C linkage variables, broad external state, or ordinary `extern` declarations.
- C++20 is technically usable for an isolated HAL prototype with the installed AVR toolchain. Production adoption remains conditional on build integration, binary measurements, hardware validation, and a stable toolchain profile.
- The accepted C++ subset uses `constexpr`, strong types, templates, concepts, and constrained RAII. It excludes dynamic allocation, exceptions, RTTI, iostreams, global objects requiring dynamic initialization, and unmeasured standard-library facilities.
- MISRA C++:2023 is applied as the safety baseline. Because that edition targets C++17, C++20 concepts require a documented extension profile and gap analysis. Code using concepts must not be described as strictly MISRA C++:2023 compliant.

## Prioritised findings

- `P1`: The current firmware build has no `.cpp` source path or C++ final-link contract. C++ production code must not be added until those paths and their boundary checks are explicit and tested.
- `P1`: Strict MISRA C++:2023 compliance and C++20 concepts are not the same language profile. The project must select either a strict C++17 claim or a documented C++20 extension profile before production migration.
- `P1`: The installed toolchain compiles the required C++20 language subset, but does not resolve `libstdc++` or `libsupc++` archives. The initial profile must remain freestanding and reject unexpected runtime dependencies.
- `P2`: Templates do not guarantee one linked implementation for different policies. The shared control and peripheral algorithms require an out-of-line non-template core and binary verification.
- `P2`: RAII is safe only for short, deterministic scopes. Interrupt state, scheduler transitions, volatile accesses, and failure reporting require explicit restrictions and disassembly review.

## Current evidence

- `test1_init.rc` declares five drivers: scheduling timer, USART, I2C, LCD, and RTC. The scheduling timer, USART, and I2C are MCU implementations; LCD and RTC are reusable drivers.
- Each current `hal_*Control()` exposes the common life-cycle, run-level, status-bit, state, and last-error protocol through `hal_driver_control_t` and `hal_driver_control_data_t`. Driver state remains private to each implementation. LCD and RTC also depend on I2C state, and the USART last error is written by an RX ISR and remains `volatile`.
- autoCode emits named `hal_<name>Control()` calls and stores those functions in generated `mod_driver_item_t` records. `sysCall` uses the runtime function pointers for enumeration, lookup, run-level startup, and SCLI control. C++ compile-time dispatch cannot remove this catalogue without changing public behaviour.
- `mk/sources.mk` discovers and maps only `.c` firmware sources. The AVR compile and link recipes use the C compiler driver. A mixed-language implementation therefore requires explicit `.cpp` discovery, object mapping, C++ compilation, and C++ final linking.
- The installed `avr-g++` identifies itself as GCC 15.1.0 and accepts `-std=c++20`. A local ATmega2560 probe compiled and linked with `-pedantic-errors`, `-Os`, LTO, section garbage collection, disabled exceptions, disabled RTTI, disabled thread-safe local statics, and disabled `__cxa_atexit`.
- The probe exercised a language-only concept, a constrained template, a `constexpr` strong type, static assertions, a stack RAII guard, and an `extern "C"` entry point. The linked ELF had no unresolved symbols and no `.data`. The same source failed in C++17 mode specifically because concepts require C++20.
- `avr-g++ -print-file-name=libstdc++.a` and `libsupc++.a` return unresolved library names in this environment. The successful probe used no C++ standard-library facility. Library availability and cost must therefore be evaluated separately from language support.

## C++20 feasibility

The required language features work with the installed compiler, so moving the proposed prototype from C++17 to C++20 is feasible. The build must select `-std=c++20`, not `-std=gnu++20`, unless a reviewed GNU extension is required. `-pedantic-errors` should reject accidental dependencies on the GNU dialect.

GCC documents C++20 support as almost complete and states that the ABI of C++20 features is not stable before GCC 16. The current GCC 15.1.0 toolchain is therefore acceptable for a whole-program prototype in which all C++ translation units are rebuilt together, but it is not an acceptable basis for distributing C++ binary objects across compiler upgrades. TaskMate should pin the compiler version for measurements and repeat ABI and size validation before every toolchain upgrade.

This audit does not authorize C++20 modules, coroutines, ranges, library containers, or concurrency facilities. They do not solve the driver factorisation problem and would widen compiler, runtime, flash, and MISRA analysis requirements.

## Use of the language features

| Feature | Intended use | Required constraint |
| --- | --- | --- |
| `constexpr` | Validate register addresses, bit positions, bus identifiers, pin mappings, and configuration relationships at compile time | Do not present volatile register access as constant evaluation; runtime hardware effects remain explicit |
| Strong types | Prevent accidental mixing of addresses, driver identifiers, run levels, pins, time values, and raw integers | Use small `enum class` values or explicit wrapper types with fixed-width storage and explicit conversion at the C boundary |
| Templates | Generate thin instance wrappers and compile-time hardware policies from one maintained definition | Keep large algorithms in one out-of-line non-template core; inspect every specialization for flash duplication |
| Concepts | State the minimal structural contract of a driver or register policy and reject invalid policies at the point of instantiation | Use language-only requirements where practical; keep concepts private to C++ HAL headers and record them as a C++20 MISRA extension |
| RAII | Restore a bounded resource state on every ordinary scope exit, including early returns | Use only automatic objects with `noexcept` construction and destruction; never hide blocking, allocation, or uncontrolled interrupt work |

`constexpr` and concepts improve validation and diagnostics, but they do not guarantee code sharing. Each distinct template specialization may emit different code because register addresses, hooks, or policies differ. `extern template` and explicit instantiation prevent repeated emission of one specialization across translation units, but do not merge different specializations.

The preferred structure is one non-template control engine operating on an explicit instance context, with thin constrained template wrappers selecting compile-time hardware policy. This guarantees one source-level and linked body for common runtime logic while retaining compile-time validation at each instance boundary. A measured timing-critical path may remain specialized when direct constant-address instructions are required.

## C ABI and architectural boundaries

The neutral headers under `srcs/interfaces/` must remain valid C. A header shared by C and C++ may wrap only its C ABI declarations as follows:

```c
#ifdef __cplusplus
extern "C" {
#endif

hal_driver_state_t hal_exampleControl(hal_driver_control_t command,
									 hal_driver_control_data_t *data);

#ifdef __cplusplus
}
#endif
```

The `extern "C"` block controls language linkage only. It does not change ownership, visibility, memory placement, or architecture. Templates, concepts, classes, references, constructors, and strong C++ types must not cross this boundary. The exported signature remains the existing C-compatible fixed-width contract.

C++ implementation and policy headers belong under the selected HAL implementation. `interfaces/` remains dependency-neutral, autoCode and the system layers remain C during the first prototype, and generated module records continue to store the existing C ABI function pointers. No assembler alias or macro workaround is needed now that this narrow linkage exception is explicit.

## RAII profile

RAII is useful for short, local ownership of interrupt state, chip-select assertion, transaction state, or another resource that must be restored after an early return. The destructor must restore the exact previous state rather than assume a fixed enabled or disabled state. Construction and destruction must have bounded execution time visible in the call graph.

RAII must not be used to model the lifetime of statically registered drivers. Driver initialization, start, stop, and error transitions remain explicit control operations because their order and failure results are observable. Global constructors, global destructors, function-local static guards, heap ownership, exceptions, and implicit registration are prohibited in the initial profile.

An interrupt-state guard must not survive a blocking syscall, scheduler transition, context switch, or call into unknown code. RAII objects used in an ISR must be proven to compile to the required bounded instruction sequence. Destructors must be `noexcept`, must not report failures by throwing, and must not conceal volatile accesses from review.

With exceptions disabled, destructors still run on normal scope exit and early `return`; they do not provide recovery from reset, halt, undefined behaviour, or non-local control transfer. RAII therefore supports structured cleanup but does not replace the existing error and life-cycle contracts.

## MISRA C++ application

MISRA C++:2023 is titled for C++17. It can govern the C++17 subset used by the design, including ownership, initialization, conversions, templates, object lifetime, and exception-related restrictions. It does not provide complete normative coverage for C++20 concepts or other post-C++17 semantics.

TaskMate should define a C++ guideline enforcement plan before production migration. The plan must identify the exact MISRA edition, rule categories, analysis tools and their coverage, compiler diagnostics, review-only rules, accepted deviations, third-party code status, and the evidence included in the compliance summary. A compiler warning profile or one static analyser alone is not a MISRA compliance process.

For a strict MISRA C++:2023 compliance claim, compile the production C++ code as C++17 and replace concepts with C++17 detection, traits, and focused `static_assert` diagnostics. `constexpr`, strong types, templates, and RAII remain available in that profile.

For the recommended C++20 engineering profile, apply every MISRA C++:2023 rule that remains applicable, prohibit unreviewed C++20 features, add project rules for concepts, and document the standards gap. The resulting code may be described as using MISRA C++:2023 as a baseline, but not as fully compliant with MISRA C++:2023. This wording remains necessary until an applicable MISRA edition or an approved organizational compliance interpretation covers the selected C++20 features.

MISRA adoption also requires a qualified checker with explicit MISRA C++:2023 coverage. The existing `bmake cppcheck` target remains useful but must not be treated as complete MISRA enforcement without a documented coverage report.

## Proposed implementation architecture

1. Keep all existing C-visible `hal_<name>Control()` signatures and generated module records. Add guarded `extern "C"` linkage declarations for headers consumed by both languages.
2. Define one private instance context per physical peripheral. Use static storage and constant initialization. Preserve `volatile` on interrupt-shared state and preserve all existing atomicity rules.
3. Implement the seven common control operations and common state evaluation in one out-of-line non-template HAL core. Pass the instance context explicitly and preserve dependency-check ordering and last-error side effects.
4. Use strong types for C++-internal identifiers, addresses, pins, bit positions, and run levels. Convert explicitly to the existing C types only inside the ABI wrapper.
5. Use `constexpr` construction and static assertions to reject invalid hardware mappings before code generation. Do not create runtime descriptors merely to support compile-time configuration.
6. Use concepts to constrain the minimal hardware policy required by a thin wrapper. Keep policy operations explicit, bounded, and free of dynamic allocation and hidden synchronization.
7. Use explicit instantiation when the same specialization is referenced from several translation units. Route substantial operations through the shared non-template core so distinct instances do not reproduce the algorithm.
8. Apply RAII only to short scoped resources whose acquisition and release semantics are deterministic and locally reviewable. Keep driver life-cycle control explicit.

## Build changes required before implementation

- Discover selected `.cpp` files without widening source-directory ownership, map `.c` and `.cpp` to unambiguous object paths, and include both kinds in dependency generation and duplicate checks.
- Compile C sources with the existing C flags and C++ sources with a separate reviewed profile based on `-std=c++20`, `-fno-exceptions`, `-fno-rtti`, `-fno-threadsafe-statics`, and `-fno-use-cxa-atexit`.
- Link the mixed program with `avr-g++` or otherwise prove that every required runtime and termination symbol is supplied. Preserve LTO, garbage collection, map output, target isolation, and all existing boundary checks.
- Add a link-time rejection check for unexpected C++ runtime symbols, allocation functions, guard variables, exception machinery, and global constructor tables.
- Pin the compiler identity in the build manifest and treat a compiler upgrade as a required binary revalidation event.

## Validation and acceptance plan

1. Record a clean C baseline for `test1`: ELF, map, section sizes, sorted symbols, disassembly, maximum stack path, and measured control-path cycles.
2. Add host tests for every control command, invalid pointer and value, all relevant state-bit combinations, run-level mutation, last-error side effects, LCD and RTC I2C dependency ordering, and USART ISR-visible error state.
3. Build an isolated C++20 prototype containing two instances of one simple peripheral. Verify the C ABI from a C caller and verify that C++ names do not leak into generated module records.
4. Prove in the linked ELF that the common control engine and selected peripheral algorithm each have one out-of-line body. Count template wrappers and attribute their flash cost.
5. Compare `.text`, `.data`, `.bss`, stack depth, control-path cycles, register accesses, interrupt latency, constructor tables, and undefined symbols against the C baseline.
6. Run the affected autoCode tests, build guards, dependency checks, static analysis, and `bmake TARGET=test1`. Run the selected MISRA checker and archive its coverage, findings, reviewed deviations, and summary.
7. Validate USART, scheduling timer, I2C, LCD, RTC, run-level transitions, error paths, and dependency loss on an Arduino Mega. A successful build and simulator run are not sufficient hardware evidence.

Acceptance requires unchanged observable behaviour, one linked common control body, no added generic dispatch state in static RAM, no dynamic initialization, no unexpected runtime dependency, bounded RAII code, reviewed MISRA evidence, and acceptable measured flash, stack, cycle, and interrupt results. If a shared engine exceeds the accepted timing budget, retain a measured specialized path instead of claiming unconditional zero cost.

## Verdict

Moving the proposed driver factorisation from C++17 to C++20 is technically possible with the installed AVR GCC 15.1.0 toolchain. The tested feature set compiles and links for ATmega2560 without persistent data or unresolved runtime symbols. C++20 concepts provide a clearer policy contract than C++17 traits, while `constexpr`, strong types, templates, and constrained RAII fit a static embedded design.

The recommended design is a mixed C and C++ HAL: stable C ABI wrappers using the authorized `extern "C"` exception, one non-template runtime core for guaranteed code sharing, and thin C++20 policy wrappers for compile-time validation. Production adoption is conditional on build support, binary and hardware measurements, a pinned toolchain, and a documented C++ safety profile.

There is one standards choice that cannot be hidden: strict MISRA C++:2023 compliance requires the C++17 language scope, while concepts require C++20. TaskMate may either keep C++17 for a strict claim, or adopt the recommended C++20 profile with MISRA C++:2023 as a baseline plus documented C++20 extension rules. It must not claim both strict MISRA C++:2023 compliance and unrestricted C++20 concepts.

## References

- Local contracts and build evidence: `srcs/interfaces/hal_drivers.h`, `srcs/system/sysCore/sys_drivers.h`, `srcs/autoCode/tagWriters/tagWriters_drivers.c`, `srcs/user/target/test1/test1_init.rc`, `mk/sources.mk`, and `srcs/hal/arch/avr8/avr8_CC.mk`.
- GCC 15.1 documentation: [language standards](https://gcc.gnu.org/onlinedocs/gcc-15.1.0/gcc/Standards.html), [C++ dialect options](https://gcc.gnu.org/onlinedocs/gcc-15.1.0/gcc/C_002b_002b-Dialect-Options.html), and [C++20 implementation status](https://gcc.gnu.org/projects/cxx-status.html#cxx20).
- MISRA publications: [MISRA C++:2023](https://misra.org.uk/product/misra-cpp2023/) and [MISRA Compliance:2020](https://www.misra.org.uk/app/uploads/2021/06/MISRA-Compliance-2020.pdf).
