# 👨‍💻 Architecture Note — autoCode

## Historical developments
`autoCode` replaced manual allocation around `v0.10` and gained `init.rc` in `v0.20`.
Revisions `v0.24` to `v0.26` added options, tagged generation, and diagnostics.
Tag `v0.30` marks the tested host generator, bounded parsing, and versioned `init.rc` grammar.
Commit `ed4674b` moved generated bodies into target-local `.inc` files under `build/`.
Commit `b6a5129` made module source declarations drive the firmware compilation list.
Commits `721aa4b` and `a0d0943` versioned the generator API and enforced build compatibility.
Before `v0.31`, obsolete HAL startup and definition generation chains were removed end to end.

## Current implementation
`bmake` compiles autoCode 1.1 as a host tool. It accepts only `init.rc` syntax 1.4 and reads
selected module declarations, error catalogues, logical GPIO signals, and tagged destinations.

Each module declares its type, run level, and at least one existing source file or directory.
Drivers may also declare an I2C address. The same declarations feed generation and Make source
selection, so module registration and compilation share one source of truth.

The generator validates bounded tokens, names, versions, duplicate modules, errors, required
options, and exactly one instance of each required tag. It emits ten guarded `.inc` fragments in
the selected target's generated directory; tracked sources retain only stable include anchors.

Temporary files protect each destination from parse failures. Make tracks input lists and every
fragment, so missing or changed generated output triggers regeneration. Normal and sanitizer
black-box targets exercise valid inputs, malformed data, version checks, and stable replacement.

## Well-built code and implementation weaknesses
### Strengths
- Registration, source selection, errors, GPIO, and generated catalogues are checked together.
- Firmware records, stacks, names, and limits remain static and allocation-free at runtime.
- Generated files are target-isolated, untracked, and reproducible for stable ordered inputs.
- The build rejects incompatible generator and `init.rc` versions before firmware compilation.

### Remaining weaknesses
- Publication is per fragment, with no transaction or rollback across the complete generated set.
- Failure tests do not inject real open, close, remove, rename, or storage-exhaustion faults.
- Compatibility is exact; no grammar migration or multi-version reader is available.
- The build manifest records versions, but not input digests or the host toolchain identity.
