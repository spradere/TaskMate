# 🔌 Architecture Note — interface

## Historical developments
The dedicated `interfaces/` layer appeared as TaskMate matured toward portability and explicit dependency rules. Earlier hardware-oriented code was gradually refactored to isolate portable logical contracts (e.g., GPIO logical signals, common pin modes) from concrete HAL implementation details.

After v0.28, the larger system/user/HAL separation made `interfaces/` the common dependency root for the
new tree. GPIO signal IDs and module/run-level counts became generated there, directory-based header
guards were applied consistently, and the register-bit macros used by AVR drivers were moved into
`interfaces/macros.h` and subsequently hardened and renamed.

## Current implementation
`interfaces/` contains no HAL, sysCore, sysCall, service, or task includes. It currently provides:

- common GPIO modes/pulls and the target-generated logical signal enum;
- the generated error enum and criticality type;
- shared string-storage types and build-time libc/debug options;
- general bit/string helper macros and a local null definition;
- protected, generated module-count and run-level definitions used by autoCode and selected core files.

Most headers are type, enum, constant, or macro contracts. Critical module/run-level headers additionally
require `TM_SYSTEM_CRITICAL_ALLOWED`, with authorised translation units receiving that definition from
the build. This makes the directory both a portability-contract layer and a home for a small amount of
generated kernel configuration.

## Well-built code and implementation weaknesses
### Strengths
- Interface headers have no upward or target-implementation includes.
- Shared enums and storage types let HAL and upper layers agree without duplicating definitions.
- Generated error, GPIO, module, and run-level values keep selected-target metadata consistent.
- Compile guards limit access to kernel-critical generated definitions.

### Remaining weaknesses
- Portable contracts and protected generated kernel configuration share one directory, so the layer's
  public scope is broader and less neutral than its rules imply.
- `macros.h` uses GNU `__typeof__` and register-oriented size dispatch, while `define.h` provides its own
  `NULL`; these choices reduce compiler neutrality and overlap standard C facilities.
- Contracts remain incomplete for timers, serial transport, scheduling context, ISR safety, optional HAL
  capabilities, and structured error handling.
