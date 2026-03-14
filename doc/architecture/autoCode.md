# 👨‍💻 Architecture Note — autoCode

## Historical developments
`autoCode` replaced earlier manual include/alloc glue code around v0.10, then evolved strongly in v0.20+ with `init.rc` parsing and in v0.21+ with arch/mcu/board split support. In v0.24-v0.26, reliability features were reinforced: option parsing, transactional file writes, generated-tag discipline, and tighter diagnostics. This history shows a deliberate move from ad-hoc startup wiring toward a build-time source-of-truth model.

## Current implementation
`autoCode` is a host-side generator that reads one config file, resolves target (`arch/mcu/board`), imports errors and module declarations (`*.rc`, `*.err`), builds an internal module database, checks limits, and emits C code/header artifacts consumed by runtime layers. It updates tagged regions in `sysCore`, `sysCall`, and HAL auto-headers, plus generated include lists and thread tables. Runtime therefore stays static, compact, and deterministic in memory layout; system flexibility is moved to compile time.

## Well-built code and implementation weaknesses
### Strengths
- Clear single-responsibility boundary: parse config, validate, generate.
- Strong fail-fast behavior (`exit(1)` on malformed options/tokens/unknown commands) reduces silent corruption.
- Generator writes multiple layers consistently, reducing drift between declarations, counts, and init flow.
- Tagged generated regions make review and regeneration predictable.

### Weaknesses (layer leaks / dependency inversion risk)
- `autoCode` knows concrete destination files across layers (`sysCore`, `sysCall`, `hal`), so generator policies and runtime layout are tightly coupled.
- Tag-based in-place mutation of production files is robust but brittle when manual edits accidentally touch generated sections.
- Build metadata and module semantics are distributed across many files; missing/renamed paths can fail late.
- Generated dense code intentionally favors machine structure over human readability, which raises debugging cost when generation errors slip through.

## Future improvements for industrial-grade embedded RTOS
- Future improvements should focus on Add a formal schema for `init.rc/.err` (versioned grammar + compatibility checks)
- Introduce generated manifest + checksum signed artifacts to support traceability and safety audits
- Add CI differential tests: identical input ⇒ byte-identical generated outputs
- Split generator backend into stable intermediate representation (IR) + pluggable emitters to reduce direct coupling to specific C files
- Add static contract checks for dependency direction (interfaces ← HAL ← sysCall ← services/tasks), and Prepare multi-target profiles (debug/safety/min-size) and deterministic generation logs suited for certification workflows.
