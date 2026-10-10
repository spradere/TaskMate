# Architecture Note: error

## Historical developments
TaskMate replaced local strings with module-owned `*.err` catalogues between `v0.23` and `v0.26`.
After `v0.28`, generated error codes and metadata moved into neutral interfaces.
Tag `v0.30` marks checked catalogue generation and the current four-level severity model.
Tag `v0.31` marks the neutral minimal halt path and consolidation of generic system errors.
After `v0.32`, public error declarations gained API documentation while runtime lookup remained message-only.

## Current implementation
Selected catalogues declare a symbolic code, quoted message, and one of four levels: `FLOW` for normal control interruption, `WARN` for recoverable anomalies, `FAIL` for component failure, and `PANIC` for a critical condition requiring a controlled halt.

The build sorts catalogues before autoCode generates an enum and fixed catalogue. `FLOW` entries store no message. Other entries use target-selected constant-string storage. Syscalls translate HAL driver state into error codes and provide bounded message lookup.

Fatal paths call the neutral `_Noreturn` halt contract. AVR8 disables interrupts and loops permanently, while the FreeBSD host terminates through its selected implementation.

## Well-built code and implementation weaknesses
### Strengths
- Codes, messages, and levels come from checked, owner-local source catalogues.
- Duplicate names, malformed records, invalid levels, and oversized catalogues stop generation.
- Message-free `FLOW` entries preserve control semantics without consuming firmware text storage.
- Terminal handling is small, deterministic, and independent of tmLibc or scheduler availability.

### Remaining weaknesses
- Public lookup exposes message text but not severity, owner, or recovery policy.
- Driver wrappers flatten dependency failures and do not preserve a causal error chain.
- Error lookup uses an 8-bit index while the catalogue limit and growth policy are not explicit at the API.
- Halt records no persistent diagnostic context and defines no verified hardware safe-state sequence.
