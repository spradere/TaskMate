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

- parses the selected `*.rc` files into fixed-size driver and thread databases;
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
- Required options and tags are counted, and missing or duplicate declarations stop generation.
- Module type/run-level declarations, names, duplicate names, error severities, and GPIO token counts
  receive explicit validation.
- Static generated tables avoid runtime discovery and dynamic allocation in the firmware.
- Content comparison avoids rebuilding files whose generated content is unchanged.

### Remaining weaknesses
- Replacement is performed one destination at a time; a failure in a later file can therefore leave an
  earlier generated file updated. The pipeline is not transactional as a whole.
- The generator includes protected runtime headers and knows their concrete structures, tag names, and
  destination layout, creating tight coupling between host tooling and kernel internals.
- Parsing relies on fixed-size buffers and a custom tokenizer without a versioned input schema. Some
  limit checks and file-comparison paths need stronger boundary and failure testing.
- There is no automated proof that identical inputs produce byte-identical outputs, nor a manifest that
  records input checksums and generator version.
