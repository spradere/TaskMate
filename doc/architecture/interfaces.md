# 🔌 Architecture Note — interfaces

## Historical developments
`interfaces/` emerged as portable contracts were separated from hardware implementations.
After `v0.28`, generated GPIO, module, run-level, string, and error contracts moved here.
Commits `66661d2` and `f6a6fa1` normalized shared headers and generic driver APIs.
Commit `e804075` replaced thread-status defines with typed bit positions.
Before `v0.31`, halt, atomic, context, GPIO, and driver contracts replaced `hal/public` relays.
Commit `776edf5` made interfaces the sole neutral boundary to selected HAL implementations.

## Current implementation
The layer contains no dependency on HAL implementation, sysCore, sysCall, tmLibc, services, tasks,
or target configuration. It owns neutral driver protocols, logical GPIO operations, opaque context,
atomic and halt contracts, common types, run levels, string descriptors, and generated catalogues.

Generated fragments provide error codes, signal identifiers, module counts, and selected limits.
Driver state and control use one compact protocol, while driver-specific operations remain dedicated
HAL functions rather than generic control commands.

The architecture matrix makes this the only deliberately transversal layer. HAL and portable system
layers may consume permitted contracts; tasks remain behind service, sysCall, or tmLibc APIs.

## Well-built code and implementation weaknesses
### Strengths
- Shared contracts are independent of concrete target headers and higher runtime layers.
- Opaque mechanism APIs let sysCore use context, atomic, and halt services without AVR details.
- Generated errors, signals, counts, and module limits track the selected composition.
- Interfaces add no runtime registration, allocation, or independent dispatch layer.

### Remaining weaknesses
- The module contract combines driver protocol, thread status, constants, and generated counts.
- Stack representation still relies on a target type injected through compiler configuration.
- String descriptors expose RAM versus ROM storage to otherwise portable consumers.
- Contracts do not generally express capabilities, ISR safety, timing, or asynchronous failures.
