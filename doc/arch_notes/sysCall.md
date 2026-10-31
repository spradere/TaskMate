# Architecture Note: sysCall

## Historical developments
`sysCall` became the task-visible boundary for kernel state and logical hardware operations.
After `v0.28`, GPIO and driver calls followed the sysCore and HAL ownership split.
Tag `v0.31` marks focused syscall groups, storage-aware strings, and target-neutral hardware calls.
After `v0.32`, separate driver and thread databases gained metadata and lifecycle syscalls.
After `v0.32`, stack-depth reporting and generated driver capability support were added.
After `v0.32`, atomic USART error snapshots and atomic logical GPIO toggle closed interrupt races.

## Current implementation
Five source groups mediate driver and peripheral operations, error lookup and halt, logical GPIO, storage-aware strings and console output, and thread, run-level, stack, and software-counter operations.

Driver syscalls use generated metadata and neutral HAL contracts for lifecycle, LCD, RTC, I2C, and USART. Thread syscalls validate public IDs and pointers, protect shared state with short atomic sections, and delegate storage and scheduling policy to sysCore.

String syscalls read bounded RAM or program-memory descriptors, compare and copy text, and transport console bytes. tmLibc stays above this boundary, while hardware calls remain target-neutral.

## Well-built code and implementation weaknesses
### Strengths
- Task-visible APIs expose cooperation and timing metadata while separating kernel policy from hardware implementations.
- Shared counters, run levels, lifecycle changes, GPIO toggle, and RX error snapshots use focused atomic sections.
- Thread names, IDs, run levels, pointers, and bounded-string entry points validate their public inputs.
- I2C discovery preserves declared state when scanning fails or overflows its fixed buffer.

### Remaining weaknesses
- Run-level driver start returns no aggregate result and discards individual failures.
- A thread's first start call only records its run level, so activation semantics remain implicit and surprising.
- GPIO set and get expose no error result for invalid or unsupported signals.
- RTC startup data has no validity flag when capture fails, is unavailable, or has not run.
