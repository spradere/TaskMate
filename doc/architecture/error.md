# 🚨 Architecture Note — error

## Historical developments
TaskMate initially handled failures in a local, ad-hoc way inside each module. As the project grew and module count increased, this approach made diagnostics inconsistent and difficult to maintain. Around v0.23-v0.26, the project introduced a system-wide error model: modules declare symbolic error entries in `.err` files, `autoCode` validates and aggregates them at build time, and runtime code consumes a generated catalog through `sysCall/error.*`. This transition moved error ownership from scattered string literals toward a centralized contract.

## Current implementation
The current design combines build-time generation and runtime lookup. During generation, `globalError()` parses `errors_all.err`, checks duplicates, validates criticality (`LOW/MID/HIGH`), and writes `src/sysCall/auto_error_catalog.h` with `err_codes_t`, `err_item_t`, and `ERROR_COUNT`. Runtime `error.c` contains a generated tagged section that builds a ROM string table and `error_catalog[]`, while `err_getMessage()` provides bounded lookup by numeric code. Callers mostly propagate `err_codes_t` and optionally print diagnostic messages through logging/services.

## Well-built code and implementation weaknesses
### Strengths
- Strong centralized namespace for error identifiers, reducing cross-module naming collisions.
- Build-time validation catches malformed entries and duplicates before firmware execution.
- Static catalog/data layout is deterministic and embedded-friendly (no heap dependency).
- Separation between generated declarations (`auto_error_catalog.h`) and runtime accessor API (`error.h`).

### Weaknesses (layer leaks / dependency inversion risk)
- Error criticality is generated but currently underused by runtime policy; severity metadata is present without enforcement semantics.
- `err_getMessage()` returns text only, encouraging string-centric handling instead of structured recovery behavior.
- Catalog generation is tightly coupled to `autoCode` flow and tagged source regions, so manual edits in generated areas are fragile.
- Error handling policy remains distributed across services/tasks; there is no unified escalation path (retry, degrade mode, safe state).

## Future improvements for industrial-grade embedded RTOS
- Future improvements should focus on defining a formal runtime error policy that maps `err_critical_t` to deterministic actions (log-only, task restart, subsystem quarantine, or controlled fail-safe)
- extending syscall APIs to expose structured error metadata beyond message strings
- adding compile-time rules that enforce ownership and prefix conventions per module
- introducing trace correlation (timestamp, task id, run level, source module) for post-mortem analysis
- and implementing fault-injection tests that verify bounded and predictable behavior under repeated error conditions.
