# Architecture Note: autoCode

## Historical developments
`autoCode` replaced manual allocation around `v0.10` and gained `init.rc` in `v0.20`.
Versions `v0.24` to `v0.26` added options, tagged generation, and diagnostics.
Tag `v0.30` marks bounded parsing, a tested host generator, and versioned `init.rc` syntax.
Tag `v0.31` marks target-local generated fragments and removal of obsolete HAL generation paths.
After `v0.32`, module sources, SCLI commands, driver availability, and separate driver and thread records became generated contracts.
After `v0.32`, autoCode gained staged publication, explicit file errors, stricter tokenization, and syntax 1.10.

## Current implementation
`bmake` compiles autoCode 1.6 as a host tool. It accepts `init.rc` syntax 1.10 and reads the selected module declarations, SCLI commands, error catalogues, logical GPIO signals, and tagged destinations.

Each module declares its type, run level, source file, and thread stack or optional driver address as applicable. These declarations drive both generated registration and the firmware compilation list.

The generator validates bounded tokens, names, versions, duplicates, source paths, options, and tag cardinality. It stages 15 guarded `.inc` fragments below the selected target build directory, then compares and publishes them after all inputs have been processed.

Normal and sanitizer black-box tests cover valid generation, malformed input, version checks, source declarations, stable replacement, cleanup, and generated contracts.

## Well-built code and implementation weaknesses
### Strengths
- Registration, source selection, errors, GPIO, driver capabilities, and SCLI commands share validated target inputs.
- Target-declared module and error capacities select checked 8-bit or 16-bit generated count types.
- Firmware records, stacks, names, limits, and real-time thread metadata remain static and allocation-free at runtime.
- Target-isolated staging prevents parse failures from replacing destinations and has broad black-box regression coverage.

### Remaining weaknesses
- Publication is still per fragment, so a rename failure can leave a mixed old and new generated set.
- Temporary names are predictable per destination, so concurrent generation for one target is unsafe.
- Fault tests do not inject real allocation, open, write, close, rename, or storage-exhaustion failures.
- The host database reserves maximum-size module arrays independently of the configured target capacity.
