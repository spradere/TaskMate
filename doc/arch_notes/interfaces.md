# Architecture Note: interfaces

## Historical developments
`interfaces/` emerged as portable contracts were separated from hardware implementations.
After `v0.28`, generated GPIO, module, run-level, string, and error contracts moved here.
Tag `v0.30` marks normalized shared headers and common driver lifecycle protocols.
Tag `v0.31` marks neutral halt, atomic, context, GPIO, and driver contracts without `hal/public` relays.
After `v0.32`, timer-context, delay, and generated driver-capability contracts were added.
After `v0.32`, public declarations gained Doxygen contracts and low-level byte APIs moved to `uint8_t`.

## Current implementation
The layer has no dependency on HAL implementation, sysCore, sysCall, tmLibc, services, tasks, or target configuration. It owns neutral driver protocols, logical GPIO, opaque context, atomic and halt contracts, common types, run levels, strings, and generated catalogues.

Generated fragments provide error codes, signals, module counts, and driver availability. Driver state and control use a compact common lifecycle protocol, while driver-specific operations remain dedicated HAL functions.

The architecture matrix makes interfaces the only deliberately cross-cutting layer. Build-selected type and string headers complete target-dependent representations only for explicitly allowed consumers.

## Well-built code and implementation weaknesses
### Strengths
- Shared contracts are independent of concrete target headers and higher runtime layers.
- Opaque mechanism APIs let sysCore use context, atomic, halt, delay, and timer services without AVR details.
- Generated errors, signals, counts, and capability flags track the selected composition.
- Interfaces add no runtime registration, allocation, or independent dispatch layer.

### Remaining weaknesses
- Module limits, generated counts, and type identifiers remain grouped in one broad contract.
- Context and stack representations still require target headers injected through compiler options.
- String descriptors expose RAM versus ROM storage to otherwise portable callers.
- Contracts rarely state ISR safety, blocking behaviour, timing bounds, or asynchronous failure semantics.
