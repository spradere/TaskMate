# 📞 Architecture Note — sysCall

## Historical developments
`sysCall` became the task-visible boundary for kernel state and logical hardware operations.
After `v0.28`, GPIO and driver calls followed the sysCore/HAL split.
Commit `26359ac` separated driver, error, GPIO, string, and thread responsibilities.
Commits `830116e` and `9ad9cf2` added initialization and RTC startup services.
Commits `9c64446` and `58958b8` moved storage-aware strings here and removed the tmLibc cycle.
The pre-`v0.31` GPIO and HAL-facade work made all hardware calls target-neutral.

## Current implementation
Five focused groups mediate driver and peripheral operations, errors and halt, logical GPIO,
storage-aware strings and console output, and thread/run-level/time-counter operations.

Driver syscalls use generated module metadata and neutral HAL contracts for lifecycle, LCD, RTC,
I2C discovery, and USART. Thread syscalls protect shared AVR state with short atomic sections and
delegate database and scheduling policy to sysCore. GPIO calls reach the neutral HAL signal API.

String syscalls read RAM or program-memory descriptors, compare and copy bounded text, and transport
console bytes. Build-selected macros create target-appropriate descriptors at call sites. tmLibc is
strictly above this boundary; syscall sources no longer include or call it.

## Well-built code and implementation weaknesses
### Strengths
- Task-visible APIs separate kernel policy from selected hardware implementations.
- Shared counters, run levels, metadata, and RX error snapshots use explicit atomic sections.
- Driver, thread, pointer, and bounded-string entry points validate key public inputs.
- I2C discovery preserves declared state when scanning fails or overflows its fixed buffer.

### Remaining weaknesses
- Run-level driver start returns no aggregate result and discards individual failures.
- Thread start accepts an unchecked initial run level and has fragile saved-level semantics.
- GPIO operations expose no invalid-signal result and toggle is not atomic.
- RTC startup data has no validity flag when capture fails or has not run.
