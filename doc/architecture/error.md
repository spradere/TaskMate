# 🚨 Architecture Note — error

## Historical developments
TaskMate initially handled failures in a local, ad-hoc way inside each module. As the project grew and module count increased, this approach made diagnostics inconsistent and difficult to maintain. Around v0.23-v0.26, the project introduced a system-wide error model: modules declare symbolic error entries in `.err` files, `autoCode` validates and aggregates them at build time, and runtime code consumes a generated catalog through `sysCall/error.*`. This transition moved error ownership from scattered string literals toward a centralized contract.

After v0.28, error declarations moved with their owners into the separated system and HAL trees. The
build now discovers and sorts the selected target's `*.err` files, while recent naming and include-path
cleanup kept the generated enum usable by HAL drivers, services, and sysCall code.

## Current implementation
Each non-comment `*.err` line declares a symbolic name, a quoted message, and a `LOW`, `MID`, or `HIGH`
criticality. The build concatenates the selected files into `build/<target>/errors_all.err`; autoCode
rejects duplicate names and unknown criticality values, then generates:

- `err_codes_t` and `ERROR_COUNT` in `interfaces/error_catalog.h`;
- ROM-backed messages and `err_item_t` entries in `system/sysCall/error.c`.

HAL drivers and services return `err_codes_t` values directly. The runtime API currently exposes only
`err_getMessage(uint8_t)`, which returns the generated string pointer for an in-range code and a null
pointer otherwise. Criticality is stored in the catalog but is not exposed through a public accessor or
used to select a runtime response.

## Well-built code and implementation weaknesses
### Strengths
- Symbolic codes, messages, and criticality originate from one generated catalog.
- Duplicate names, malformed declarations, and invalid severity words are detected during
  generation.
- The firmware uses fixed-size enum/table data and ROM-backed text rather than runtime allocation.
- Message lookup checks its index before reading the generated array, and HAL operations now reject
  use when their driver lifecycle state is not running.

### Remaining weaknesses
- The only public lookup returns a message; callers cannot query criticality, error owner, or a
  prescribed recovery action through the API.
- Error signaling is fragmented between `err_codes_t`, raw `uint8_t` lifecycle returns, and
  `DRV_STATE_*` values. Success is not uniform, and many boot, service, LCD, RTC, and I2C call sites
  discard downstream failures.
- Output pointers remain unchecked in USART read, I2C read, RTC read/write, and message-channel
  allocation paths, so some reported errors still coexist with unchecked memory access.
- The generator exposes a 256-slot catalog limit while lookup and several loops use 8-bit indexes;
  terminal-count handling and the ABI extension policy are undocumented.
- There is no structured runtime record containing context, occurrence count, timestamp, or
  originating module, and no tested escalation path from driver failure to safe state.
