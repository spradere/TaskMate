# 🚨 Architecture Note — error

## Historical developments
TaskMate replaced local strings with module-owned `*.err` catalogues between `v0.23` and `v0.26`.
After `v0.28`, generated error codes and metadata moved into neutral interfaces.
Commits `7ee2725` and `08771cb` established the four current severity levels.
Commit `79ac629` removed `FLOW` text from firmware ROM while preserving its symbolic codes.
Commit `f78057f` replaced formatted panic handling with a minimal neutral halt contract.
After `v0.31`, generic system errors were consolidated in `system/general.err`.

## Current implementation
Selected catalogues declare a symbolic code, quoted message, and one of four levels: `FLOW` for
normal control interruption, `WARN` for recoverable anomalies, `FAIL` for component failure, and
`PANIC` for a critical condition requiring a controlled halt.

The build sorts declarations before autoCode generates the enum and fixed catalogue. `FLOW` entries
have no stored message; other entries use target-appropriate constant text. Syscalls translate HAL
driver state into error codes and provide bounded message lookup to services.

Fatal paths call the neutral `_Noreturn` halt contract. The AVR8 implementation disables interrupts
and loops permanently, without formatting, USART output, allocation, or scheduler dependence.

## Well-built code and implementation weaknesses
### Strengths
- Codes, messages, and levels come from checked, owner-local source catalogues.
- Duplicate names, malformed records, and invalid levels stop generation.
- Message-free `FLOW` entries preserve normal control semantics without consuming text storage.
- Terminal handling has a small deterministic contract independent of higher layers.

### Remaining weaknesses
- Public lookup exposes message text but not severity, owner, or recovery policy.
- Driver wrappers flatten dependency failures and do not preserve a causal error chain.
- Error indices and several consumers remain 8-bit without an explicit growth policy.
- Halt records no persistent diagnostic context and has no verified hardware safe-state sequence.
