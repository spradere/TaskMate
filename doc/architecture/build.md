# 🏗️ Architecture Note — build

## Historical developments
TaskMate’s build system has evolved from a single entry Makefile into a layered BSD `bmake` stack split by concern: global orchestration (`Makefile`), reusable infrastructure (`mk/*.mk`), and target-dependent extension points (`src/hal/**/**_make.mk`). The current structure reflects two parallel needs: (1) deterministic firmware build for embedded targets and (2) host-side project tooling (autoCode generation, static checks, docs, editor helpers, and backup workflows).

A key step in this evolution is the integration of `autoCode` as a first-class build phase and the explicit `ARCH -> MCU -> BOARD` target validation path. Together, these choices move configuration errors and integration drift from runtime toward build time.

## Current implementation
The build architecture is a staged pipeline with strong variable-driven composition.

### 1) Entry-point composition and layering
`Makefile` defines root directories, imports option/target validation early, computes target build output directory (`build/<arch>_<mcu>_<board>`), then includes path/data/color/rule modules plus hardware-specific make fragments. This gives a predictable load order:
- options and target constraints first,
- file discovery/data synthesis next,
- generic rules and utilities,
- architecture/mcu/board-specific compiler/link/upload behavior last.

### 2) Target model and validation
`mk/hardware_target.mk` defines defaults (`avr8/atmega2560/arduinoMega`) and validates compatibility at each level (`VALID_ARCHS`, `VALID_MCUS.<arch>`, `VALID_BOARD.<mcu>`). Invalid combinations fail fast with `.error`, reducing accidental cross-target builds.

### 3) Build graph and lifecycle hooks
`mk/build.mk` sets `.MAIN: all` and uses `.BEGIN`/`.END` hooks:
- `.BEGIN` ensures build/log directories exist and updates target-scoped version stamp.
- `.END` emits a `last_build_info.txt` artifact with version/date/target/build counter/git/avr-gcc metadata.

The `all` target pipeline is:
1. `_system_critical_check`
2. `${AUTOCODE_STAMP}`
3. `_dependency_check`
4. `${TARGET}`

This ordering enforces policy checks and code generation before compilation/linking.

### 4) Source discovery and generated data
`mk/sources_data.mk` centralizes dynamic discovery using `find`:
- project C/H sources from scoped source directories,
- autoCode sources,
- `init.rc` and `*.err` inventories,
- HAL headers annotated with `// @hal_user` or `// @hal_system`,
- make/doc file sets used by editor/help targets.

It also materializes build state (dependency include file, timestamped autoCode logs, build counter, upstream branch).

### 5) Paths, artifacts, and VCS boundaries
`mk/path_files.mk` defines named artifacts for build outputs, logs, tags, generated config, HAL lists, and error catalog. It also encodes a generated `.gitignore` policy (`mk/backup.mk`) that defaults to “ignore everything, then whitelist allowed trees/patterns,” minimizing accidental commits.

### 6) Build policy and safety checks
`mk/header_allow.mk` and `_system_critical_check` in `mk/build.mk` implement include-governance:
- specific modules receive compile-time macros enabling critical HAL access,
- include patterns (`hal/auto_hal_system.h`, `hal/auto_hal_init.h`) are scanned,
- only allowlisted files may include those headers.

Violations fail the build, acting as an architectural boundary check.

### 7) Toolchain specialization by hardware layer
HAL make fragments add target-specific settings:
- `arch_make.mk` selects compiler (`avr-gcc`), warning profile, optimization/LTO flags, defines metadata macros, and provides link/compile/upload/dump/memory targets.
- `mcu_make.mk` adds `-mmcu` selection.
- `board_make.mk` adds clock/programmer/port details.

This keeps common logic generic while isolating hardware variability.

### 8) Developer utility surface
`mk/utils.mk`, `mk/editors.mk`, and `mk/backup.mk` provide operational tooling:
- clean/doc/cloc/note/cppcheck/clang-format/clang-tidy/help,
- ctags generation and editor launchers,
- git push/pull/merge helpers and USB rsync backup.

The help system extracts documented targets via `#@` annotations in make fragments.

## Well-built code and implementation weaknesses
### Strengths
- Strong modular decomposition of build concerns (options, paths, discovery, policy, utilities, hardware).
- Fail-fast validation for target compatibility and forbidden critical includes.
- Deterministic build artifacts with target-scoped directories and generated metadata.
- Build-time code generation (`autoCode`) is embedded in the main dependency chain, reducing runtime/config mismatch.
- Hardware specialization is cleanly layered (`arch`/`mcu`/`board`) and composable.

### Weaknesses (layer leaks / dependency inversion risk)
- Build logic is BSD `bmake`-specific (`.include`, `.if`, `.for`, `.sinclude`), reducing portability to GNU Make-centric environments.
- Several rules embed environment-specific absolute include paths (clang-tidy/cppcheck), which can drift across developer machines.
- Some backup/git targets are interactive (`read`) and branch-hardcoded (`merge test`), limiting CI/non-interactive reproducibility.
- `_system_critical_check` uses recursive `grep`, which is practical but coarse (string-match policy vs semantic include graph).
- Build metadata in `.END` is AVR-centric (`avr-gcc -dumpversion`) even when non-AVR targets are configured.

## Future improvements for industrial-grade embedded RTOS
- Future improvements should focus on introducing a target-agnostic toolchain abstraction (per-arch tool info providers) so metadata and checks stay valid for non-AVR platforms.
- Replacing host-specific absolute analysis include paths with generated compile databases (`compile_commands.json`) and standardized tool wrappers.
- Making backup/git helper targets fully non-interactive and CI-safe (optional prompts, explicit branch parameters).
- Strengthening policy checks from text grep to parser-assisted include validation and architecture rule tests.
- Adding reproducibility controls: pinned tool versions, deterministic environment report, and optional “strict mode” that fails on unstaged generated artifacts.
