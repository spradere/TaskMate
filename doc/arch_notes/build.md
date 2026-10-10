# Architecture Note: build

## Historical developments
TaskMate evolved from one Makefile into BSD `bmake` orchestration and focused `mk/*.mk` files.
After `v0.28`, hardware selection became an explicit target, board, MCU or host, and architecture stack.
Tag `v0.30` marks integrated autoCode tests, syntax versioning, and architecture include checks.
Tag `v0.31` marks target-local generated fragments and build-fatal architectural boundary checks.
After `v0.32`, target makefiles were normalized and the FreeBSD simulation became the `z600` target.
After `v0.32`, source lists reject duplicates and cppcheck plus clang-tidy checks were integrated.

## Current implementation
The configured targets are `test1` and `noscli` on Arduino Mega, ATmega2560, and AVR8, plus `z600` on the PC, FreeBSD, and ucontext simulation stack. `test1` is the reference validation target.

Firmware sources combine the sysCore, sysCall, boot, and target-selected support files with explicit module and SCLI source declarations from the selected `init.rc`. Lists are sorted, checked for duplicates, validated against the repository boundary, and mapped to target-local objects.

The normal pipeline validates programs and the hardware stack, checks autoCode compatibility, regenerates target-local fragments, enforces direct-include rules, builds dependencies and firmware, and reports source and memory data. Separate targets run autoCode tests, build tests, cppcheck, and clang-tidy.

Artifacts, generated code, logs, manifests, stamps, and dependency files live below `build/`. Destructive utilities use a repository-confinement check before deleting generated paths.

## Well-built code and implementation weaknesses
### Strengths
- Hardware selection, source ownership, generation, boundary checking, compilation, and reporting are separated.
- Module declarations drive both autoCode validation and the firmware compilation list.
- Duplicate paths, missing sources, incompatible versions, forbidden includes, and unsafe utility paths fail early.
- AVR and host stacks reuse one composition pipeline while retaining target-specific compiler settings.

### Remaining weaknesses
- Only `test1` is the required reference build, so regressions in `noscli` or `z600` can escape routine validation.
- The pipeline depends on BSD `bmake` and a substantial Unix or BSD host-tool set.
- Recursive `-source_dir` declarations can silently widen a module's compilation boundary.
- Several Make-owned files are replaced independently, so interruption can leave a partially updated build state.
