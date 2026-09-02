# 👨‍💻 Architecture Note — autoCode

## Historical developments
`autoCode` replaced earlier manual include/alloc glue code around v0.10, then evolved strongly in v0.20+ with `init.rc` parsing and in v0.21+ with arch/mcu/board split support. In v0.24-v0.26, reliability features were reinforced: option parsing, per-file temporary replacement, generated-tag discipline, and tighter diagnostics. This history shows a deliberate move from ad-hoc startup wiring toward a build-time source-of-truth model.

After v0.28, the system/user/HAL directory reorganisation changed the generator inputs. The selected
hardware target now provides ordered header and source lists through the build system, while
`signals.gpio`, typed `*.rc` entries, and the global `*.err` catalog feed additional generated regions.
Recent work also normalised generated identifiers and kept generated zones compatible with the
project formatting and static-analysis rules.

## Current implementation
`bmake` compiles `srcs/autoCode/` as a host program with Clang and writes a target-specific
configuration file containing the TaskMate version/build number and paths to input lists. The program
then:

- parses the selected `*.rc` files, whose module entries contain separate `-type <data>` and
  `-run <data>` pairs, into fixed-size driver and thread databases;
- aggregates `*.err` declarations and their `LOW`, `MID`, or `HIGH` criticality;
- reads the selected HAL/target header lists and `signals.gpio`;
- rewrites the tagged regions in module, run-level, error, GPIO, HAL include, and system-information
  files.

Each destination is copied to a `.tmp` file, regenerated, compared with the existing file, and replaced
only when its content changed. A target-scoped stamp makes generation a prerequisite of dependency
collection, compilation, and linking. The generated data fixes module counts, stacks, function tables,
run-level tables, error codes, and logical GPIO identifiers at build time.

## Well-built code and implementation weaknesses
### Strengths
- Required options and output tags are counted; missing, duplicated, or unknown declarations stop
  generation.
- Module types, run levels, same-type duplicate names, error severities, and GPIO line token counts
  receive explicit validation before the firmware is compiled.
- Generated records include fixed thread contexts, saved run levels, driver control callbacks, and
  ROM-backed names, avoiding runtime discovery and dynamic allocation.
- Generation is integrated into the dependency graph, produces a reviewable log, and preserves an
  existing destination when its generated content is unchanged.

### Remaining weaknesses
- Replacement is performed one destination at a time. It removes the old file before renaming the
  temporary file, ignores `remove()`/`rename()` failures, and cannot roll back earlier replacements.
- Fixed-size parsing silently truncates excess tokens and long lines. Module-name and module-count
  checks contain boundary conditions that can admit an out-of-bounds terminating byte or array
  index.
- File comparison uses `feof()` before checking the result of `fgets()`, so unequal-length or failed
  reads can compare stale buffer contents. Paths copied with `strncpy()` are not explicitly
  terminated.
- The host generator includes runtime interface definitions and emits concrete kernel structures,
  include paths, status encodings, and callback names, tightly coupling both sides of the build.
  Error levels are nevertheless shared through `interfaces/error_level.h`; autoCode does not
  redefine them.
- There is no automated valid/invalid corpus, boundary test suite, failure-injection test, or
  manifest recording input hashes and generator/tool versions.
