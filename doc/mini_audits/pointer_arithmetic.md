# Pointer arithmetic in TaskMate

Date: 11 October 2026
Version: `v0.32`
Revision: `a0a52133d2301f3bf2d909081e5e395e70c36845`

## Question

Where does current non-legacy C code move pointers explicitly, and which uses need bounds review?

## Problems found in code

- [AVR context setup](../../srcs/hal/arch/avr8/avr8_context.c) uses `sp--` to build the initial stack frame. AVR context restore and the [Timer1 context switch](../../srcs/hal/mcu/atmega2560/at2560_timerContext.c) use `Z+` to advance a register pointer. These operations depend on the exact stack layout and AVR calling convention.
- [ucontext setup](../../srcs/hal/host/ucontext/ucontext_context.c) adds one stack word to `stack_top`, then subtracts the usable stack size. A wrong bound could include a canary or exclude usable stack.
- Indexed addresses such as `&table[index]` also occur in [autoCode](../../srcs/autoCode/initrcCommands/addScliCommand.c), [HAL](../../srcs/hal/host/ucontext/ucontext_context.c), and [sysCore](../../srcs/system/sysCore/sys_threads.c). They require valid indexes, but their syntax alone does not indicate a defect.

## Solution

Keep the explicit stack pointer operations in the context implementations. Verify stack bounds, canary placement, and saved registers when changing them. Keep table access indexed and check each index against its owning capacity.
