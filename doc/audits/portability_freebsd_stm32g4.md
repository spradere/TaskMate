# FreeBSD/amd64 and STM32G4/Nucleo portability audit

Audit date: 2026-09-20

Analysed revision: `25747c4` (`tag, update layers view`)

## Scope and verdict

This audit assesses two new targets without changing the code:

- a native functional simulation on FreeBSD/amd64 using `ucontext`;
- an embedded port to an STM32G4 Cortex-M4F and a Nucleo board.

It updates the earlier FreeBSD analysis without modifying that
[historical snapshot](amd64_freebsd_ucontext.md).

Both ports are **feasible**, but neither is a simple compiler change. The verdict is
**conditional GO**. First, the context and stack contracts must become truly independent of
AVR, the 16-bit ABI assumptions in formatting must be fixed, then each hardware stack must be
added within the existing `arch`, `mcu`, `board`, and `target` layers.

The FreeBSD port is the shortest path to reproducible functional tests. STM32G4 is the better
test of embedded portability and real-time behaviour, but it requires on-board validation, a
startup and linker script, and a carefully verified Cortex-M context switch.

## Existing favourable boundaries

- [hal_context_t](../../srcs/interfaces/hal_context.h#L21) is opaque to the kernel, and
  [hal_contextStart()](../../srcs/interfaces/hal_context.h#L31) already confines the first
  start to HAL.
- The scheduler calls a timer through the neutral `drv_timerSched.h` interface. Its
  [round-robin](../../srcs/system/sysCore/sys_scheduler.c#L96) policy accesses no AVR register.
- GPIO, USART, I2C, and timers have neutral contracts. The LCD and RTC drivers can therefore
  be retained if the new I2C backend matches their semantics exactly.
- The `target -> board -> mcu -> arch` selection and autoCode source lists allow
  implementations to be added without hardware `#if` directives in portable code.
- Thread stacks, contexts, and records are static. STM32 needs no dynamic allocator.
  autoCode remains a host tool and does not need to be ported to the MCU.

## High-priority common blockers

| Priority | Observed blocker | Consequence | Proposed solution |
|---|---|---|---|
| P0 | [hal_threadContextInit()](../../srcs/interfaces/hal_context.h#L28) receives only the stack top | `makecontext()` requires base and size; alignment and capacity cannot be validated | Pass base and size in bytes, or a neutral descriptor, and generate the call through autoCode |
| P0 | [Fixed size of 256](../../srcs/interfaces/tm_modules.h#L56) `hal_stack_word_t` values | 256 bytes on AVR, but different meaning and cost on amd64 and ARM | Move the size to the target, express capacity in bytes, and enforce architecture alignment |
| P0 | [Direct assignment](../../srcs/system/sysCore/sys_scheduler.c#L102) of the context | Copying a `ucontext_t` may retain internal pointers and fixes HAL semantics in the kernel | Add a HAL capture or copy primitive, or pass the permanent current-context location to the callback |
| P0 | [tm_vsnprintf()](../../srcs/tmLibc/stdio/tm_snprintf.c#L176) reads `%i` with `va_arg(args, uint16_t)` | Variadic promotion supplies an `int`; the AVR 16-bit assumption is incorrect on ILP32 and LP64 | Read the promoted type, `int` or `unsigned int`, apply explicit bounds, and add 32-bit and 64-bit tests |
| P1 | Complete `hal_context_t` and `hal_stack_word_t` types are injected with `-include` | The type is source-opaque, but `mod_thread_item_t` needs its size; every toolchain depends on a forced include | Keep an architecture-selected type header, but test its presence, alignment, and consistency before autoCode |
| P1 | Final build is AVR-specific: `.elf`, `.hex`, `avr-size`, and upload | Neither a host executable nor an ARM image can follow the current rules | Provide architecture-specific compile and link fragments, with dedicated memory, image, and flash targets |
| P1 | [USART](../../srcs/system/TaskMate.c#L38) is required before the scheduler, followed by I2C, RTC, and LCD at run-level startup | A minimal backend cannot start the current application | Provide these capabilities, or create a minimal test target with its own `init.rc`, without a fake kernel driver |
| P1 | Two time sources are assumed: 1 ms quantum and 10 ms STC | An incorrectly multiplexed single tick drifts or changes callback order | Use either two timers, or one monotonic base with a documented divider and expiry catch-up |
| P2 | Some public sizes and counters remain limited to 8 bits | Not currently blocking, but silent limits change on 32-bit and 64-bit hosts | Keep embedded bounds, add assertions, and do not blindly replace them with `size_t` |

These changes are system-critical. The autoCode sources, generated regions, generator tests,
and AVR build must be changed and validated together.

## Option A: amd64/FreeBSD simulation with `ucontext`

### Proposed structure

```text
srcs/hal/arch/amd64/          context, signals, atomics, halt, native build
srcs/hal/mcu/freebsd/         timers and virtual peripherals
srcs/hal/board/hostSim/       process and terminal initialisation
srcs/user/target/simFreeBSD/  init.rc, virtual GPIO, and application choices
```

The name `mcu/freebsd` adapts the current hardware taxonomy. It represents host OS services,
not a microcontroller. A future simulator family might justify a `platform` layer, but it is
not needed for this first port.

### Contexts and scheduling

`getcontext()` initialises each `ucontext_t`, then `makecontext()` assigns its entry point,
`uc_stack.ss_sp`, `uc_stack.ss_size`, and completion link. `setcontext()` is suitable for
the first start. A returning task must go to an explicit fatal trampoline because a TaskMate
task is not expected to return.

Two levels are possible:

1. **Cooperative MVP**: `hal_timerSchedLoad()` performs a controlled switch. This is simple
   and useful for testing autoCode, run levels, and syscalls, but does not validate preemption.
2. **Preemptive simulation**: a monotonic timer delivers a signal installed with
   `SA_SIGINFO`. The handler receives the interrupted context, invokes policy, then lets the
   signal return restore the selected context.

`swapcontext()` must not be assumed to be async-signal-safe. The handler must be minimal and
must call neither stdio nor allocation. The exact design for modifying the signal context must
be validated on the selected FreeBSD version. The signal must be blocked during
initialisation, selection, and every shared non-atomic update.

### FreeBSD blockers and solutions

| Blocker | Possible solution | Required validation |
|---|---|---|
| `ucontext` was removed from POSIX.1-2008 | Explicitly adopt FreeBSD as the platform and isolate all calls in `arch/amd64` | Compile on supported FreeBSD versions |
| Host stacks are too small | Target size of several dozen KiB, with an optional `mmap` and `mprotect` guard page | Maximum depth, nested signals, intentional overflow |
| `ucontext_t` copying and lifetime | Permanent per-thread storage and a HAL capture primitive; do not expose its representation to the kernel | Registers, signal mask, FPU and SIMD under optimisation |
| Lost tick when the process is delayed | Monotonic timer and overrun count; catch up STCs without an unbounded burst | CPU load, debugger pause, long-term drift |
| AVR atomic section equals a global mask | Return a token containing the old mask and block only TaskMate signals | Nesting, handler call, exact restoration |
| `tm_stdio.h` selects libc by target | Decide whether simulation tests tmLibc or libc; do not mix both in one oracle | Shared AVR and host formatting tests |
| Missing peripherals | USART on a non-blocking terminal; traceable GPIO state; virtual I2C bus with RTC and LCD, or a minimal target | Scripted scenarios and injected errors |
| Non-deterministic host scheduling | Virtual-clock mode without signals for CI, plus a separate preemptive suite | Repeatable CI and stress tests |

### What the simulation does and does not prove

It can validate round-robin policy, run levels, syscalls, STCs, services, tasks, SCLI, and
virtual peripheral errors. It cannot prove SRAM or flash footprint, IRQ latency and priority,
volatile-register semantics, Harvard separation or `PROGMEM`, electrical behaviour, or MCU
real-time guarantees.

## Option B: STM32G4 port on a Nucleo board

### Reference target to fix

`STM32G4` and `Nucleo` do not identify one target. The first milestone must fix the MCU and
board references and the board revision. **NUCLEO-G474RE** is a consistent candidate:
STM32G474RE, Cortex-M4F, ST-LINK connector, and Nucleo-64 format. USART, I2C, LED, button,
crystal, and solder-bridge pins must come from the board user manual, not Arduino assumptions.

Proposed organisation:

```text
srcs/hal/arch/armv7em/          exceptions, context, atomics, halt, toolchain
srcs/hal/mcu/stm32g474/         clocks, NVIC, TIM/SysTick, GPIO, USART, I2C
srcs/hal/board/nucleoG474RE/    board startup, clock, and ST-LINK/VCP pinout
srcs/user/target/nucleoG474RE/  init.rc and logical signals
```

The architecture layer must remain `armv7em`, not `stm32`, so the Cortex-M mechanism can
serve other MCUs. STM32 registers and vectors remain in `mcu`. Pin assignments remain in
`board` and `target`.

### Cortex-M4F context switch

The recommended path uses PSP for threads, MSP for handlers, SysTick or a TIM for the tick, and
PendSV at the lowest priority for the actual switch:

1. the exception automatically stacks R0-R3, R12, LR, PC, and xPSR;
2. PendSV saves R4-R11 and the PSP in the current context;
3. the portable callback selects the next context;
4. PendSV restores R4-R11 and the PSP, then returns from the exception;
5. `hal_contextStart()` loads PSP and CONTROL, then performs a synthetic exception return.

The initial stack must meet the 8-byte AAPCS alignment, provide xPSR with the Thumb bit, a
correctly paired PC, and an LR that points to the fatal trampoline. If the FPU is allowed, the
contract must specify lazy saving and the S16-S31 registers. Disabling it in TaskMate firmware
for the first milestone greatly reduces risk. `DSB` and `ISB` barriers must be placed as
specified by the ARM manual when masks, priorities, and contexts change.

### STM32G4 blockers and solutions

| Blocker | Possible solution | Required validation |
|---|---|---|
| No ARM startup, vector table, or linker script | Add an MCU startup unit, flash and RAM sections, `.data` copy, `.bss` clearing, stack symbols, and `Reset_Handler` | Map file, debugger reset, and standalone start |
| Monolithic AVR save in the timer ISR | Separate the tick source from PendSV; keep the round-robin decision in `sysCore` | Sentinel registers, preemption at each sensitive instruction |
| FPU and conditional stacking | Disable FPU for the MVP, or use an extended context driven by EXC_RETURN and FPCA | Floating-point tasks, IRQ, lazy stacking |
| Atomics based on SREG | Implement the token with PRIMASK, or BASEPRI if only TaskMate IRQs are masked; exactly restore the incoming state | Nesting and calls from ISRs |
| Clocks and ticks fixed for 16 MHz | Calculate dividers from the effective frequency, with error assertions and explicit units | GPIO and logic-analyser measurement at 1 ms and 10 ms |
| Drivers only for ATmega2560 | Implement STM32 GPIO, USART, and I2C under existing contracts; retain LCD and RTC above them | NACK, timeout, stuck bus, concurrent RX and TX |
| AVR `-fshort-enums` and objects shared with ISRs | Do not depend on native enum size; use fixed storage and explicit conversions | `sizeof`, conversion warnings, and LTO |
| Unmeasured fixed stacks | Size by target and thread, use sentinel filling and high-water measurement, retain canaries | Maximum load and nested IRQs |
| AVR build and `avrdude` upload | `arm-none-eabi-gcc`, CPU and Thumb options, linker script, `objcopy`, and `size`; flash with STM32CubeProgrammer or OpenOCD | Clean build, flash, reset, and SWD debug |
| Possible dependency on vendor libraries | Explicitly choose CMSIS alone or LL; confine STM32 headers to HAL and pin versions and options | Include checks and reproducibility |
| Existing modules at 3.3 V on Nucleo | 3.3 V I2C pull-ups, voltage and supply checks, adaptation if needed | On-board measurements and module datasheets |

### Timer choices

Two designs are suitable:

- SysTick at 1 ms requests PendSV, and a divider calls the STC every 10 occurrences;
- one TIM produces the quantum and a second TIM produces the STC, at the cost of one more
  peripheral.

The first minimises code and provides a shared time base. The second separates the two
contracts more clearly and more closely resembles AVR. In both cases, context switching must
remain in PendSV, and the order of simultaneous processing must be documented.

## Comparison and recommended strategy

| Criterion | FreeBSD/amd64 | STM32G4/Nucleo |
|---|---|---|
| Main value | Functional CI, diagnostics, error injection | MCU portability and real-time behaviour |
| Initial effort | Medium | High |
| Dominant risk | Signals and `ucontext` semantics | Exceptions, ABI, FPU, startup, and clocks |
| Determinism | Strong with a virtual clock, weak in host real time | Strong if IRQ and priorities are correctly designed |
| Hardware validation | None | Required on the exact reference |
| Reuse | Fast tests of all upper layers | ARMv7E-M base for other MCUs |

Recommended order:

1. ~~add host tests for variadic formatting and fix the `int == uint16_t` assumption;~~
2. evolve stack, context, and autoCode, then prove AVR non-regression;
3. deliver a cooperative FreeBSD target with a virtual clock;
4. add FreeBSD signals and preemption in a separate stress suite;
5. fix NUCLEO-G474RE and deliver startup, linker, USART, and GPIO without a scheduler;
6. add PSP and PendSV, plus a minimal two-thread test without FPU;
7. add STC, I2C, RTC and LCD, timing measurements, and stack-overflow tests;
8. enable the FPU only after an explicit decision about floating-point context.

## Exit criteria

### Common

- functional, unchanged AVR build, stable autoCode, and no forbidden-layer include;
- stack-contract tests for base, size, alignment, canaries, and fatal task return;
- formatting tests on 16-bit, 32-bit, and 64-bit ABIs;
- no reference to the new hardware outside its target, HAL, and build selector.

### FreeBSD

- reproducible deterministic cooperative scenario in CI;
- preemption under load, mask restoration, and STC without unbounded drift;
- ASan and UBSan execution of the compatible mode, plus halt and stack tests;
- explicit documentation of supported FreeBSD versions.

### STM32G4

- reproducible build, controlled flash and RAM budget, and checked map file;
- general-purpose registers preserved across preemption and yield;
- measured 1 ms and 10 ms ticks, verified PendSV priority, and atomics tested with nested IRQs;
- cold boot, USART through ST-LINK, GPIO, I2C, RTC, and LCD validated on the board;
- measured stack margin for every thread and documented fault behaviour.

## External reference sources

- [FreeBSD `getcontext(3)` and `makecontext(3)`](https://man.freebsd.org/cgi/man.cgi?query=getcontext&sektion=3)
- [FreeBSD `sigaction(2)`](https://man.freebsd.org/cgi/man.cgi?query=sigaction&sektion=2)
- [FreeBSD POSIX timers](https://man.freebsd.org/cgi/man.cgi?query=timer_create&sektion=2)
- [STMicroelectronics NUCLEO-G474RE](https://www.st.com/en/evaluation-tools/nucleo-g474re.html)
- [STMicroelectronics STM32G474RE](https://www.st.com/en/microcontrollers-microprocessors/stm32g474re.html)
- [ST RM0440, STM32G4 reference manual](https://www.st.com/resource/en/reference_manual/rm0440-stm32g4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [Arm CMSIS-Core, Cortex-M4](https://arm-software.github.io/CMSIS_6/latest/Core/group__CMSIS__Core.html)

Pin numbers, alternate functions, maximum frequencies, and revisions must be rechecked in the
MCU datasheet and the user manual for the selected board revision at implementation time.
