# 🏗️ Architecture Note — build

## Historical developments
TaskMate evolved from one Makefile into BSD `bmake` orchestration, focused `mk/*.mk` fragments, and
target-owned HAL fragments. autoCode and target validation became first-class build phases.

After tag `v0.28`, the source split made `HWT -> BOARD -> MCU -> ARCH` explicit. Commit `7ae12ca`
moved configuration and validated targets; `v0.29` (`9fc9513`) consolidated build rules.

Header allow-list parsing, warnings, and role-based variable names were then tightened around the
current AVR build pipeline. Commit `a2a7c65` added the integrated autoCode regression targets;
`e1d320a` made startup-header and startup-function lists separate generated inputs.

## Current implementation
Configuration declares the default `test1` target and a `test_noscli` composition for the same
Arduino Mega, ATmega2560, and AVR8 stack. The latter is intended to omit SCLI. System-wide and
target-owned module declarations are discovered separately and combined by autoCode.

Source selection separates discovery from compilation. `PATHS_SOURCE_SEARCH` remains the broad
directory list used to find headers, `init.rc`, error catalogues, and other build inputs. The
compilation list is assembled explicitly from four sources:

- the fixed base in `system/sysCore/`, `system/sysCall/`, `system/TaskMate.c`, and `tmLibc/`;
- module-owned files named by `-source_file` in the selected `*_init.rc` files;
- every C file below the directories named by `-source_dir` in those files;
- target-selected non-module sources contributed through `FILES_EXTRA_SRC` by target, board, MCU,
  and architecture Make fragments.

The resulting `FILES_COMPILE_SRC` list is sorted and deduplicated before object paths are derived.
Startup translation units and the ATmega2560 GPIO implementation use `FILES_EXTRA_SRC`; module
implementations stay attached to their `init.rc` declaration. The AVR8 directory remains attached
to `timerSched` as the current special case.

The normal build checks tools and the hardware stack, regenerates autoCode, and verifies guarded
headers. It then collects dependencies, builds AVR firmware, and reports memory use and line counts.
Target artefacts, generated lists, logs, and stamps remain under `build/`.

Each selected target, board, MCU, and architecture fragment contributes its startup header and
function. autoCode emits their includes and calls in architecture-to-target initialization order.

Generic driver headers in `interfaces/` are explicit autoCode dependencies. The header checker scans
sources against `conf/system_header_allow.conf`, while compile-time guards protect critical headers.
Dedicated targets run autoCode by validation stage or as a complete black-box corpus, with an
ASan/UBSan build available for host-side memory and undefined-behaviour checks.

The direct-include matrix now records `tmLibc` as a horizontal layer and permits service access to
neutral interfaces. The checker does not yet classify `srcs/tmLibc/`, so those libc relations are
documented target rules but are not build-enforced until a future code change extends the checker.

## Well-built code and implementation weaknesses
### Strengths
- Orchestration, discovery, hardware selection, checks, and utilities are separated by concern.
- Architecture, MCU, board, and target fragments contribute only their selected responsibilities.
- Missing target data, generated inputs, HAL selection, or guarded access fails before execution.
- AVR builds and host autoCode tests use broad warnings and explicit diagnostic reporting.

### Remaining weaknesses
- Invalid-target diagnostics still name the obsolete selector instead of the active target variable.
- Direct `tmLibc` dependency rules are present in the matrix but not yet checked.
- Unsorted `*.rc` discovery can still make autoCode input ordering depend on filesystem enumeration.
- Only one hardware stack exercises portability; the build also assumes BSD and Unix tooling.
