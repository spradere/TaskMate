# 👨‍💻 Architecture Note — autoCode

## Historical developments
`autoCode` replaced earlier manual include/alloc glue code around v0.10, then evolved strongly in v0.20+ with `init.rc` parsing and in v0.21+ with arch/mcu/board split support. In v0.24-v0.26, reliability features were reinforced: option parsing, transactional file writes, generated-tag discipline, and tighter diagnostics. This history shows a deliberate move from ad-hoc startup wiring toward a build-time source-of-truth model.

## Current implementation
`autoCode` parses target/config inputs (`ARCH/MCU/BOARD`, `*.rc`, `*.err`), validates module/error definitions, and regenerates tagged runtime artifacts consumed by sysCore/sysCall/HAL integration points. It updates only generated regions/files and remains central to deterministic static module layout.

## Well-built code and implementation weaknesses
### Strengths
- Strong fail-fast behavior for malformed configuration.
- Single generation pipeline keeps module/error metadata coherent across layers.
- Tagged generation provides predictable regeneration boundaries.

### Remaining weaknesses
- Generator still knows multiple concrete destination files, so coupling to runtime file layout remains high.
- In-place tagged updates stay sensitive to accidental manual edits in generated zones.
- IR/schema versioning is still mostly convention-driven.

## Future improvements for industrial-grade embedded RTOS
- Introduce a versioned intermediate schema/IR with strict compatibility checks.
- Add deterministic output checks in CI (same input => byte-identical output).
- Emit generation manifests (inputs, checksums, target fingerprint) for traceability.
