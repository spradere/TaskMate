# Prerequisites for C++ in TaskMate

Date: 11 October 2026
Version: `v0.32`
Revision: `c58a966487551f5e975122584b93f974be35d46f`

## Question

Which weaknesses recorded in `doc/arch_notes/` must be addressed before the first mixed C/C++ implementation on `test1`?

## Problems found in code

- P0: The [source list](../../mk/sources.mk) discovers directory sources only as `.c` and maps only `.c` to objects. The [AVR recipe](../../srcs/hal/arch/avr8/avr8_CC.mk) compiles and links with `avr-gcc` and C flags. A declared `.cpp` file cannot enter a correct C++20 build. The build tests also assume C-only object mapping.
- P0: Shared contracts do not yet compile as C++20. An `avr-g++ -std=c++20` syntax check fails on `_Noreturn` in [hal_context.h](../../srcs/interfaces/hal_context.h) and `_Static_assert` in [tm_string.h](../../srcs/interfaces/tm_string.h). Public function declarations such as those in [drv_timerContext.h](../../srcs/interfaces/drv_timerContext.h) also lack conditional C linkage, so a C++ caller would use a different symbol name.
- P1: The [AVR context setup](../../srcs/hal/arch/avr8/avr8_context.c) fixes the third return-address byte at zero. The [target configuration](../../srcs/hal/mcu/atmega2560/at2560_make.mk) therefore budgets 64 KiB of flash. C++ code growth must be checked against the linked image; the existing memory report is a measurement, not a demonstrated link-time guard.
- Other noted defects are component-specific: [tmLibc formatting](../../srcs/tmLibc/stdio/tm_snprintf.c) uses shared static state, [GPIO](../../srcs/hal/mcu/atmega2560/at2560_gpio.c) lacks some mapping checks, and [startup](../../srcs/system/sysCall/sc_driver.c) discards some driver errors. They do not block an isolated HAL C++ pilot that does not change those paths. The `sysCall` note's unchecked run-level claim is stale: [sc_threadStart](../../srcs/system/sysCall/sc_threads.c) now checks the range.

## Solution

Before adding a C++ component, extend source selection, object mapping, dependency checks, compilation, linking, and build tests for `.cpp` with separate C++20 flags. Make every header and generated contract used at the selected C/C++ boundary compile in both languages, with C linkage only for intentional function ABI boundaries. Then build a small mixed-language `test1` pilot and compare its final ELF, map, symbols, disassembly, RAM, stack, timing, and ISR path with a direct C baseline; keep the image within the AVR context limit. Fix the remaining architecture-note defects when their owning component is changed, not as a prerequisite for unrelated C++ work.
