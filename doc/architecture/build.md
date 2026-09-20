# 🏗️ Architecture Note — build

## Historical developments
TaskMate evolved from one Makefile into BSD `bmake` orchestration and focused `mk/*.mk` files.
After `v0.28`, hardware selection became the explicit target, board, MCU, and architecture stack.
Tag `v0.30` marks integrated autoCode tests, syntax versioning, and architecture include checks.
Commit `b6a5129` replaced broad firmware discovery with declarations from selected `init.rc` files.
Commit `ed4674b` isolated generated fragments below each target build directory.
Commit `776edf5` removed the `hal/public` facade and made boundary checks build-fatal.
After `v0.31`, the build manifest gained both autoCode and `init.rc` version reporting.

## Current implementation
The sole configured target is `test1`, selecting Arduino Mega, ATmega2560, and AVR8. Its Make
fragments contribute target configuration, hardware settings, compiler flags, and selected sources.

Firmware sources combine the sysCore/sysCall base and boot entry point with module files and
directories declared in `test1_init.rc`. Target fragments add tmLibc, architecture support, and the
MCU GPIO implementation. The final list is sorted and deduplicated before object mapping.

The normal pipeline validates tools and the hardware stack, rejects retired HAL facade tokens,
checks the autoCode API version, regenerates target-local fragments, and enforces direct-include
and critical-header rules. It then builds dependencies and AVR firmware and reports size data.

Build artifacts, generated code, logs, manifests, and stamps live below `build/`. Destructive Make
utilities pass paths through a type-aware repository-confinement guard. The final manifest records
TaskMate, hardware, Git, compiler, autoCode, and `init.rc` versions.

## Well-built code and implementation weaknesses
### Strengths
- Hardware selection, source ownership, generation, checking, compilation, and reporting are split.
- Module source declarations drive both autoCode validation and firmware compilation.
- Removed facades, forbidden includes, incompatible versions, and unsafe utility paths fail early.
- Host tests, sanitizer tests, AVR warnings, and deterministic target-local outputs are integrated.

### Remaining weaknesses
- Only AVR8/ATmega2560/Arduino Mega exercises the build and portability contracts.
- The pipeline assumes BSD `bmake` and several Unix or BSD host utilities.
- Recursive `-source_dir` expansion can silently widen a module's compilation boundary.
- Some discovery lists depend on filesystem enumeration before later sorting or generation.
