# 🧩 Architecture Note — services

## Historical developments
Services were introduced after early core scheduling work to provide reusable system-level threads (message server, serial CLI) without mixing application code and kernel internals. Over time, services became autoCode-managed modules with run-level metadata and generated allocation.

After v0.28, services moved into `srcs/system/services` as part of the explicit system/user split. Their
headers and generated registration were adapted to the new include layout. More recently, the message
and CLI loops adopted the cooperative-yield syscall while waiting on software time counters, reducing
their deliberate spin time between polling cycles.

## Current implementation
`services_init.rc` registers two system threads at `RUN_SERVICE`. autoCode assigns each a fixed thread
record and stack:

- `msg` owns four static 64-byte channels. Producers reserve a channel, copy text into it, select LCD,
  USART, or null output, and set a send flag. The `msg` thread scans all channels and calls the selected
  HAL backend directly.
- `scli` reserves one message channel, polls the HAL USART receive buffer, assembles up to 127 received
  bytes into a local line, and submits that line to `msg` for USART output.

Both service loops set their current thread's software counter to 100 ticks and call `sc_coopYield()`
while waiting. Resources are fixed at compile time; there is no heap allocation, blocking queue, or
service registry beyond the generated module database.

## Well-built code and implementation weaknesses
### Strengths
- Service threads, stacks, channels, and message buffers have fixed memory costs.
- autoCode gives services the same explicit type and run-level metadata as other modules.
- The cooperative wait path allows another thread to run instead of intentionally consuming every slice.
- Message routing and CLI parsing are kept outside the scheduler and kernel data structures.

### Remaining weaknesses
- Both services bypass syscall mediation for USART/LCD access, coupling reusable system services to the
  current HAL and selected devices.
- Channel reservation, writing, processing, and release have no atomic protection or ownership model;
  indexes and output pointers are not validated.
- The message service performs synchronous HAL output, mixes transport with LCD-specific presentation,
  and does not consistently clear send state for every destination. A failed initial reservation can
  also leave the local channel value undefined before release.
- There is no bounded queue/backpressure policy, delivery result, drop counter, timeout, or fairness
  guarantee. The CLI is an echo prototype rather than a command parser and still polls the UART.
