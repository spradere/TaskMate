# 📚 Architecture Note — tm_libc

## Historical developments
`tm_libc` emerged to avoid full libc dependency on constrained targets and to control code size/behavior. Over revisions, it gained formatted output support (`tm_snprintf`, `tm_vsnprintf`) and system logging helpers integrated with target-specific output backends.

## Current implementation
`tm_libc` provides compact utility primitives:
- string helpers (`tm_string.*`),
- stdio-like formatting (`tm_stdio.*`, `tm_snprintf.*`),
- logging facade (`tm_syslog.*`).

The logging path is currently thin: `tm_syslog` forwards variadic arguments to `tm_vprintf`, with platform selection handled elsewhere (HAL-generated include strategy).

## Well-built code and implementation weaknesses
### Strengths
- Embedded-focused lightweight replacement strategy.
- Reduces dependency on host/toolchain libc variability.
- Unified string type/macros support ROM-aware string handling patterns.

### Weaknesses (layer leaks / dependency inversion risk)
- Logging backend indirection is minimal; policy (levels, sinks, buffering) mostly absent.
- `tm_libc` includes can propagate broadly, risking hidden cross-layer coupling if not constrained.
- Limited formal tests for edge cases (formatting bounds, locale assumptions, reentrancy).

## Future improvements for industrial-grade embedded RTOS
- Future improvements should focus on Define strict reentrant subset + ISR-safe formatting variants
- Add compile-time selectable log levels/sinks with bounded-time guarantees
- Build unit/property tests for formatter correctness and truncation safety, and Document and enforce ABI stability for `tm_string_t` and formatting APIs.
