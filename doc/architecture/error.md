# 🚨 Architecture Note — error

## Historical developments
TaskMate initially handled failures in a local, ad-hoc way inside each module. As the project grew and module count increased, this approach made diagnostics inconsistent and difficult to maintain. Around v0.23-v0.26, the project introduced a system-wide error model: modules declare symbolic error entries in `.err` files, `autoCode` validates and aggregates them at build time, and runtime code consumes a generated catalog through `sysCall/error.*`. This transition moved error ownership from scattered string literals toward a centralized contract.

## Current implementation
Error flow combines build-time generation and runtime lookup:
- `.err` inputs are merged and validated by `autoCode`.
- generated catalog/types are provided through interface-visible headers.
- runtime lookup remains in `sysCall/error.*` with bounded indexed access.

## Well-built code and implementation weaknesses
### Strengths
- Single global namespace for error codes.
- Duplicate/malformed entries are caught before firmware build completes.
- Static catalog layout is deterministic and memory-safe for embedded use.

### Remaining weaknesses
- Runtime policy still treats errors mostly as diagnostics, not action contracts.
- Severity metadata is available but underexploited in recovery flow.
- Escalation ownership across services/tasks/kernel remains fragmented.

## Future improvements for industrial-grade embedded RTOS
- Map severities to deterministic runtime actions (retry/degrade/safe-state).
- Expose richer structured metadata (module, context, timestamp correlation).
- Add fault-injection and recovery-path tests in CI.
