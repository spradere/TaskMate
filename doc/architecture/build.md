# 🏗️ Architecture Note — build

## Historical developments
TaskMate’s build system has evolved from a single entry Makefile into a layered BSD `bmake` stack split by concern: global orchestration (`Makefile`), reusable infrastructure (`mk/*.mk`), and target-dependent extension points (`src/hal/**/**_make.mk`). The current structure reflects two parallel needs: (1) deterministic firmware build for embedded targets and (2) host-side project tooling (autoCode generation, static checks, docs, editor helpers, and backup workflows).

A key step in this evolution is the integration of `autoCode` as a first-class build phase and the explicit `ARCH -> MCU -> BOARD` target validation path. Together, these choices move configuration errors and integration drift from runtime toward build time.

## Current implementation
The build pipeline remains staged and deterministic:
1. target/options validation (`ARCH/MCU/BOARD` compatibility),
2. code generation (`autoCode` stamp-driven),
3. dependency checks,
4. compile/link/output artifacts.

`mk/*.mk` files separate concerns (paths, source discovery, policy checks, utilities, hardware-specific flags), while target-specialized files in `hal/**/**_make.mk` inject platform details.

## Well-built code and implementation weaknesses
### Strengths
- Clear decomposition of build concerns.
- Strong fail-fast validation for invalid targets and boundary violations.
- Deterministic artifact placement and target-scoped metadata.
- Architecture checks now better aligned with dependency-direction goals.

### Remaining weaknesses
- Heavy BSD `bmake` usage limits portability to GNU Make-first environments.
- Some tooling paths/settings remain host-specific.
- Text-based include checks are useful but less robust than semantic graph validation.

## Future improvements for industrial-grade embedded RTOS
- Add semantic include/dependency graph checks to complement current grep-based guards.
- Standardize analysis tooling through generated compile databases.
- Add stricter reproducibility mode (pinned tool versions + generated artifact cleanliness checks).
