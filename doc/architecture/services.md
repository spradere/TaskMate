# 🧩 Architecture Note — services

## Historical developments
Services were introduced after early core scheduling work to provide reusable system-level threads (message server, serial CLI) without mixing application code and kernel internals. Over time, services became autoCode-managed modules with run-level metadata and generated allocation.

## Current implementation
Current service set includes:
- `msg`: bounded channel-based dispatch toward output backends.
- `scli`: serial command shell skeleton using `msg`.

Services are statically allocated and registered through generated module metadata.

## Well-built code and implementation weaknesses
### Strengths
- Clear module separation and static resource model.
- Good integration with generated init/module tables.
- Service responsibilities are distinct enough for incremental expansion.

### Remaining weaknesses
- Some services still include HAL public headers directly.
- Message path still couples routing and presentation concerns.
- Cooperative loops can still drift toward busy work under load.

## Future improvements for industrial-grade embedded RTOS
- Route hardware access through syscall façades by default (audited exceptions only).
- Separate transport backend from message formatting/protocol parsing.
- Add health metrics (queue depth, drop counters, deadline miss indicators).
