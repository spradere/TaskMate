# 📚 Architecture Note — tm_libc

## Historical developments
`tm_libc` emerged to avoid full libc dependency on constrained targets and to control code size/behavior. Over revisions, it gained formatted output support (`tm_snprintf`, `tm_vsnprintf`) and system logging helpers integrated with target-specific output backends.

After v0.28, `tm_libc` moved into the separated source tree and its include paths, Doxygen headers, and
identifier names were normalised. Formatter fixes accompanied stricter compiler warnings. In August
2026, formatting gained a cooperative-yield attempt around its shared lock so another thread could run
when the formatter was already in use.

## Current implementation
`interfaces/options.h` selects either the TaskMate implementation or standard libc aliases at compile
time. In TaskMate mode:

- `tm_string_t` records whether text is in RAM or ROM, and AVR macros place constant strings in
  `PROGMEM`;
- `tm_strncpy()` performs the project's small bounded copy;
- `tm_snprintf()`, `tm_vsnprintf()`, `tm_printf()`, and `tm_vprintf()` share one static formatter state and
  support `%c`, `%s`, `%i`, `%x`, `%b`, `%%`, and one-digit zero padding;
- `tm_syslog()` forwards variadic arguments to the formatter.

The selected `hal/public/tmlibc.h` supplies character reads for RAM/ROM strings and character output.
On ATmega2560, output is buffered through USART and flushed when full or when a newline is written. The
implementation is intentionally much smaller than a conforming C stdio/string library.

## Well-built code and implementation weaknesses
### Strengths
- ROM-aware strings avoid copying diagnostic text into scarce AVR RAM.
- The supported formatting subset is explicit and has fixed temporary storage.
- Compile-time selection keeps call sites stable between internal and standard-library modes.
- The output backend is selected through a HAL public entry point rather than AVR register access in the
  formatter itself.

### Remaining weaknesses
- Formatting uses global mutable state. If already locked it yields only once and then proceeds without
  rechecking ownership; an invalid padding path returns without clearing the lock. It is therefore not
  reentrant or ISR-safe.
- `tm_strncpy()` can write the terminator at index `n`, which exceeds a destination whose capacity is
  exactly `n`; pointer and zero-size contracts are also not validated consistently.
- Formatting supports only 16-bit unsigned conversion behavior despite `%i`, has no negative/long values,
  precision, multi-digit width, or standard `snprintf` return semantics guarantee.
- `tm_libc` depends on sysCall for cooperative yield and on the MCU-oriented HAL string contract, which
  complicates its intended position as a low-level reusable library. Logging has no levels, sink policy,
  backpressure result, or bounded-time guarantee.
