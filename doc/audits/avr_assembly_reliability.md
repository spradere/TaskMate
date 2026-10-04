# AVR Assembly Reliability Audit

Date: 2026-10-04

Scope: tracked non-legacy assembly and its direct C, ABI, stack, timer, and build contracts

Reference target: `test1`, AVR8, ATmega2560, Arduino Mega

## Executive summary

The project has a small assembly surface. It is concentrated in the AVR8 context implementation, the ATmega2560 scheduling interrupt, and the AVR8 atomic barrier. This is a good architectural boundary, but the context switch currently has two correctness defects and several contracts that are not verified by the build.

The first defect is critical. Context save clears `r1` before pushing it. AVR-GCC normally keeps `r1` zero, but instructions such as multiply may temporarily leave a non-zero value in it. An interrupt at that point must preserve the original value. The current switch instead resumes with zero and can silently corrupt the interrupted computation.

The second defect is high risk. The naked scheduling ISR contains extended assembly with operands and is split across five assembly statements. GCC documents that only basic assembly is supported in naked functions. The present output may work with one toolchain and optimization set, but it is not a supported compiler contract, especially with LTO enabled.

Stack safety is also not established. An AVR context frame is 36 bytes, while autoCode accepts a total thread stack of three bytes. The `test1` task stacks are 50 bytes. The scheduling callback runs on the interrupted thread's stack after the 36-byte context has been pushed, so it competes with the interrupted task and nested C calls for the remaining margin. Canary checks detect some overruns only after writes have already occurred.

## Assembly inventory

| Location | Purpose | Form | Risk concentration |
| --- | --- | --- | --- |
| `srcs/hal/arch/avr8/avr8_context.h` | Save and restore 32 registers and `SREG` | Basic assembly text macros | Register order and frame schema |
| `srcs/hal/arch/avr8/avr8_context.c` | Build and start an initial context | C frame construction and one naked basic assembly block | Frame symmetry and initial interrupt state |
| `srcs/hal/mcu/atmega2560/at2560_timerSched.c` | Stop the timer, call the scheduler, change stacks, restart, and return | Extended assembly macros inside a naked ISR | Compiler support, ABI, stack depth, IO addresses |
| `srcs/hal/arch/avr8/avr8_atomic.h` | Compiler barrier before restoring `SREG` | Empty extended assembly with a memory clobber | Ordering contract |

No tracked non-legacy `.S`, `.s`, or `.asm` source exists. The critical routine is therefore hidden inside C strings and cannot be assembled, inspected, or unit-tested as a distinct source unit.

## Findings

### A1, critical: `r1` is destroyed before it is saved

`AVR8_CONTEXT_SAVE` saves `r0` and `SREG`, executes `clr r1`, and only then pushes `r1`. Restore later pops the saved byte into `r1`. This records zero, not the interrupted value.

The ABI requires `r1` to be zero at normal C sequence points, but code generation may use it transiently, notably as the high result register of multiply instructions. An interrupt may occur between the instruction that changes it and the instruction that clears it. Interrupt transparency requires the sequence to push `r1` first, then clear it before calling C.

Recommended correction:

1. Change the order to save `r1` before clearing it.
2. Keep `r1` zero while the scheduler callback executes.
3. Add an instruction-level regression check for `push r1` preceding `clr r1` and for the reverse restore order.
4. Run an on-target stress test that preempts multiply-heavy code at a high scheduler frequency and checks deterministic results.

Primary ABI reference: <https://avrdudes.github.io/avr-libc/avr-libc-user-manual/FAQ.html>.

### A2, high: extended assembly is used in a naked function

The scheduling ISR is declared `ISR_NAKED`, but timer stop, callback dispatch, and timer start use operand and clobber lists. They are extended assembly. GCC explicitly states that only basic assembly is safe in naked functions and that extended assembly or mixed C and assembly is unsupported.

The five separate statements also make instruction adjacency an implicit optimizer assumption. `volatile` prevents deletion, but it does not turn the group into one compiler-supported naked-function body. LTO increases the importance of removing this assumption.

Recommended correction:

1. Move the complete ISR entry, context save, callback bridge, stack switch, context restore, and `reti` into one tracked `.S` unit.
2. Export only a normal C scheduler bridge with an explicit AVR ABI contract.
3. Use assembler-visible symbols or a deliberately generated offsets header. Do not reproduce C object layout as unexplained literals.
4. If an interim inline implementation is required, use one basic assembly body and globally named assembly symbols. Treat this only as a migration step because basic assembly cannot describe C data dependencies to the compiler.

Primary compiler reference: <https://gcc.gnu.org/onlinedocs/gcc/AVR-Attributes.html>.

### A3, high: accepted stack sizes do not satisfy the AVR context contract

The initial frame contains a three-byte program counter, `r0`, `SREG`, and `r1` through `r31`, for 36 bytes. autoCode accepts stacks from three bytes upward and passes the last usable stack byte to the architecture initializer. A legal configuration can therefore underflow the array by 35 bytes during boot.

The current `test1` task stacks are 50 bytes including two canaries. During preemption, the 36-byte saved context leaves at most 12 bytes before the low canary when the task itself has no live stack data. The scheduler callback then runs on that same stack and performs C calls. This margin is not supported by a measured worst-case stack report.

Recommended correction:

1. Define an architecture-provided initial-frame size and validate every generated thread stack against it plus both canaries.
2. Include the maximum scheduler callback call depth in the minimum or switch to a dedicated scheduler stack before calling C.
3. Make generation fail for an invalid target configuration. A runtime canary is too late for deterministic boot-time underflow.
4. Measure high-water use on hardware for every `test1` thread under worst-case interrupt timing, then retain a documented margin.

### A4, medium: the context frame schema has three unsynchronised definitions

The frame is defined by the push order, the reverse pop order, and the C writes used for a new thread. There is no named offset table, frame-size assertion, or generated test tying these definitions together. Adding an architectural register such as `RAMPZ`, changing program-counter assumptions, or editing one sequence can silently desynchronise initial and interrupted contexts.

Recommended correction:

1. Define named frame offsets and total size in one AVR-owned contract consumable by assembly and C.
2. Build initial contexts by offset rather than by an undocumented chain of post-decrements.
3. Add static assertions for pointer width, stack word width, context layout, and the expected three-byte return address on the selected MCU.
4. Add a host-side byte-layout test and an AVR disassembly check. The host test validates schema, not instruction behaviour.

### A5, medium: extended-address state is an undocumented invariant

The switch saves general registers and `SREG`, but not `EIND` or `RAMPZ`. Callback dispatch uses `eicall`, whose upper address state depends on `EIND`. The build limits reported flash to 64 KiB and the initial context forces the third program-counter byte to zero, but these restrictions are spread across comments and Make variables rather than expressed as a checked context capability.

This is acceptable only while all executable code and relevant flash accesses obey the near-address model and no code changes `EIND` or relies on thread-local `RAMPZ`. A future size increase or far-memory helper can violate that assumption without changing the switch.

Recommended correction:

1. Document and build-check the exact executable byte range supported by the context backend.
2. Fail the link or post-link validation when a code address exceeds the supported range.
3. Either reserve `EIND` and `RAMPZ` as global architecture state kept at known values, or include them in the context schema before far code or far data is allowed.
4. Use the linker result, not only the configured memory-reporting limit, as the enforcement point.

### A6, medium: hardware register names are partly replaced by numeric addresses

The callback bridge uses `0x3d` and `0x3e` for the stack pointer while the context start path uses `__SP_L__` and `__SP_H__`. The numeric form is correct for the selected device but obscures intent and makes device-header validation impossible.

Recommended correction:

1. Use device or architecture symbols in the assembly source.
2. Add assembler-time checks when an instruction requires an IO-space address rather than a data-space address.
3. Keep the low-byte and high-byte write order explicit and explain the interrupt-state precondition.

### A7, medium: the build produces disassembly but does not validate it

The `dump` target writes ELF and HEX disassembly, but no test checks the critical instruction sequences. The context source is excluded from clang-tidy because of the naked implementation. C linters cannot reason about the assembly strings, register preservation, stack-pointer writes, or `reti`.

Recommended correction:

1. Add a post-link checker that extracts the context-start routine and scheduler vector from the ELF.
2. Verify one entry path, exact save and restore sets, `r1` ordering, one scheduler call, stack-pointer transfer order, timer bracketing, and terminal `reti`.
3. Reject unexpected prologue, epilogue, stack adjustment, helper call, or duplicated ISR body.
4. Record and test the AVR compiler and binutils versions used for release artifacts.

## Positive properties to preserve

- Assembly is confined to the selected HAL architecture and MCU layers.
- Context start is non-returning and restores the selected stack in one architecture operation.
- The switch saves all 32 general registers and `SREG`, rather than relying only on ordinary call-clobbered rules.
- The callback bridge clears `r1` before entering C and describes its register and memory effects in the current extended assembly form.
- Atomic restoration uses a compiler memory barrier and restores the complete prior `SREG`, which preserves nesting semantics.
- The scheduler timer is stopped around the scheduling callback, preventing the same timer source from re-entering the switch while interrupts remain disabled.

## Proposed implementation order

1. Fix A1 and add a focused instruction-order regression test. This is the smallest correction with the highest correctness value.
2. Enforce an AVR stack minimum and increase measured `test1` margins as needed. Do not wait for the assembly migration to prevent boot-time underflow.
3. Move the complete naked scheduling path to `.S` and define one context-frame contract shared with C.
4. Add post-link disassembly checks and supported-address-range checks.
5. Decide whether to reserve or save `EIND` and `RAMPZ` before permitting firmware above the current near-address envelope.
6. Perform hardware stress validation after each context-switch change. Keep each commit small enough to compare instruction sequences and cycle counts.

## Validation matrix for the corrections

| Check | Host/build | AVR simulator | ATmega2560 hardware |
| --- | --- | --- | --- |
| Frame size and offset assertions | Required | Optional | Not applicable |
| Disassembly sequence and no compiler prologue | Required | Optional | Not applicable |
| Reject undersized generated stacks | Required | Not applicable | Not applicable |
| Preserve non-zero transient `r1` | Inspect and targeted object test | Required if available | Required |
| First start and repeated preemption | Build only | Required if available | Required |
| Cooperative and timer-triggered switch | Build only | Required if available | Required |
| Worst-case stack high-water | Estimate | Useful | Required |
| Timer period and switch latency | Instruction count | Useful | Required with measurement |
| Nested atomic state and ISR interaction | Static checks | Useful | Required |

## Audit limitations

`bmake TARGET=test1` could not run in the audit environment because `bmake` was not installed. AVR compiler and objdump tools were also unavailable, so no current ELF, disassembly, cycle count, or stack-depth measurement was produced. The findings are based on source-level instruction and ABI analysis. A successful build alone would not close A1 or A2. Both require instruction-level and hardware validation.
