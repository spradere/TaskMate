# 📚 Architecture Note — tm_libc

## Historical developments
`tm_libc` emerged to avoid full libc dependency on constrained targets and to control code size/behavior. Over revisions, it gained formatted output support (`tm_snprintf`, `tm_vsnprintf`) and system logging helpers integrated with target-specific output backends.

## Current implementation
`tm_libc` provides compact primitives:
- strings (`tm_string.*`),
- formatting (`tm_stdio.*`, `tm_snprintf.*`),
- logging façade (`tm_syslog.*`).

Backends remain target-selected through HAL/public integration points.

## Well-built code and implementation weaknesses
### Strengths
- Small footprint and predictable behavior for embedded constraints.
- Shields core code from libc/toolchain variability.
- Provides consistent project-level string/formatting conventions.

### Remaining weaknesses
- Logging policy (levels/sinks/backpressure) is still thin.
- Reentrancy/ISR-safety guarantees are not fully formalized for all functions.
- Boundary-testing depth for formatter edge cases can be improved.

## Future improvements for industrial-grade embedded RTOS
- Define ISR-safe/reentrant subsets explicitly.
- Add compile-time log policy configuration with bounded-time paths.
- Expand formatter/property tests for truncation and boundary behavior.
