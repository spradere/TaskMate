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
- The cooperative wait path allows another thread to run instead of intentionally consuming every
  slice.
- SCLI uses fixed line/argument bounds, table-driven dispatch, RAM/ROM-aware comparisons, and
  explicit thread/driver list and lifecycle commands through syscalls.
- Message routing and command parsing remain outside scheduler policy and kernel data structures.

### Remaining weaknesses
- The temporary service-to-HAL bridge remains across the two services for USART and LCD access. It
  must be removed rather than extended with new direct hardware dependencies.
- Channel reservation, writing, processing, and release have no atomic protection or ownership
  model;
  channel indexes and output pointers are not validated. Startup frees an undefined or stale channel
  ID after a failed initial reservation.
- Message delivery is synchronous, ignores HAL results, embeds LCD presentation, and clears the send
  bit only for USART; LCD and null destinations can therefore be processed repeatedly.
- SCLI polls the UART and processes each received chunk immediately instead of accumulating a
  terminated line. `scli_line_length` is unused, long input is split, and excess arguments are
  silently truncated.
- There is no queue/backpressure policy, delivery result, drop counter, timeout, or fairness
  guarantee for producers or transports.
