# ncurses console performance audit

## Purpose and scope

This audit analyses the reading, processing, and display of the simulated USART on the
`ucontext / pc / freebsd` target. It covers `pc_console.c`, its USART caller, GPIO display,
and SCLI consumption. It corresponds to commit `9b396a9` and changes no behaviour.

The AVR target does not use ncurses. The recommendations therefore concern the fidelity and
responsiveness of the host simulator, without introducing this mechanism into portable layers.

## Method and limitations

The static analysis followed these paths:

```text
SCLI -> sc_usartRead() -> hal_usartRead() -> pc_consoleReadByte() -> wgetch()
tmLibc -> sc_consoleFlush() -> hal_usartSendTXBuffer() -> waddch() / wrefresh()
GPIO -> pc_consoleLedWrite() -> mvwprintw() / wrefresh()
```

The code provides neither counters nor an ncurses performance benchmark. The audit environment
is Linux, while the implementation uses FreeBSD signals and `sigreturn()`. The priorities
below are therefore based on call counts and critical sections, not on times presented as
measurements. A FreeBSD campaign in a pseudo-terminal is required before and after any fix.

## Current operation

- ncurses is initialised once, and six windows are retained statically.
- Input is non-blocking. An empty `wgetch()` call ends the draining of ready keys.
- Each printable character or deletion redraws the entire line with `werase()`,
  `waddnstr()`, then `wrefresh()`.
- A submitted line is copied into a second 64-byte buffer. SCLI then reads it byte by byte,
  with one signal mask and restore pair for each byte.
- Output accumulates up to 256 bytes in the USART. Flushing calls `waddch()` for each byte,
  followed by one `wrefresh()`. tmLibc normally triggers this flush at the end of each line.
- Each GPIO write immediately reformats and refreshes its line, even when its value is
  unchanged. It also polls the keyboard.

## Ranked findings

### P1: ncurses runs inside long critical sections

`hal_usartRead()` masks signals before calling the ncurses read operation. The first call can
drain all available keys and refresh the screen several times. Output also keeps signals masked
throughout the 256-byte loop and its `wrefresh()`. Finally, GPIO updates include
`mvwprintw()` and `wrefresh()` inside a critical section.

These calls may calculate screen differences and perform terminal writes whose duration is not
bounded by TaskMate. They therefore delay the 1 ms scheduling signal and distort simulated time
precisely when input and output are active. This is the main risk.

The critical sections must not simply be removed. Signal-based context switches could then
enter ncurses from another thread while windows and buffers are shared. The recommended fix is
a single console owner:

1. producers place bytes or GPIO state in fixed buffers;
2. a console service routine processes a bounded budget between scheduling points;
3. no other context calls ncurses;
4. only short queue-index mutations are protected.

This change must remain in the host HAL. A dynamic queue, a POSIX mutex exposed to higher
layers, or a change to the USART contract would exceed the architectural scope.

### P2: input refreshes the terminal for every key

For a burst of `n` characters, the current path performs `n` erases, rewrites a total of
`1 + 2 + ... + n` characters, and requests `n` refreshes. Rebuilding is therefore quadratic
within a line, although the 63-byte maximum bounds it.

A local, low-risk fix is to process all available keys, mark the line as modified, then call
`consoleInputRender()` once. Submission must remain immediate, and the routine must stop
draining when the ready buffer is occupied. Replacing `wrefresh()` with `wnoutrefresh()` and
grouping windows under one `doupdate()` can then combine terminal writes.

### P3: the byte-oriented contract multiplies receive-side signal masking

SCLI calls `sc_usartRead()` until it receives the "buffer empty" error. Each byte already
copied into `input_ready` still causes `hal_atomicStart()` and `hal_atomicEnd()`. A
63-byte line therefore causes at least 64 mask changes in each direction, 63 of which no longer
touch ncurses.

Because the public USART contract intentionally remains byte-oriented, the recommended first
step is to move the ready-buffer test and copy into a short path without another ncurses poll.
A block HAL read should be considered only in a separate architectural request, with the same
tests on AVR and FreeBSD.

### P4: GPIO refreshes are immediate and sometimes unnecessary

`pc_consoleLedWrite()` reformats the line and calls `wrefresh()` on every write. The
`signal_values` cache already exists, but the caller overwrites it before a comparison can be
made. In addition, an LED update triggers keyboard input, coupling two unrelated paths.

Writes whose value has not changed should be ignored. Modified lines should be marked and
rendered with the next console batch. Initialisation must still draw every line. Keyboard
polling should have an explicit call site and no longer depend on GPIO activity.

### P5: output is already grouped, but its policy remains implicit

The 256-byte USART buffer and end-of-line refresh normally avoid one `wrefresh()` per
character. This path is therefore better than input and GPIO. Two limitations remain: the loop
calls `waddch()` for every byte, and a line longer than 256 bytes fails before tmLibc reaches
its newline.

The first objective should be to move rendering outside the critical section, not to replace
the loop. Only then should `waddnstr()` be compared with `waddch()` for blocks without null
bytes. The byte-by-byte version remains more faithful to the binary contract and is the
reference.

## Improvement plan

1. Add instrumentation compiled only for the benchmark: keys drained, line renders,
   `wrefresh()` and `doupdate()` calls, RX and TX bytes, and maximum critical-section
   duration.
2. Group rendering for an input burst and verify content, cursor position, backspace, Enter,
   F10, empty lines, and saturation at 63 bytes.
3. Remove unchanged GPIO renders, then group input, output, and GPIO with `wnoutrefresh()`
   and one `doupdate()` per batch.
4. Introduce a single ncurses owner and limit critical sections to fixed queues. Explicitly
   preserve byte order and the current rejection policy on saturation.
5. Optimise the TX loop or RX contract only after measuring the first four steps.

## Required benchmark

The benchmark must run on FreeBSD in a fixed-size pseudo-terminal, with the same `TERM` and
locale. Each scenario must be repeated at least 30 times after warm-up:

| Scenario | Load | Main measurements |
|---|---|---|
| Interactive RX | lines of 1, 32, and 63 bytes | key-to-display latency, render calls |
| Burst RX | paste of 1,000 lines | throughput, CPU, lost lines |
| TX | blocks of 1, 64, and 256 bytes | throughput, refreshes, byte order |
| GPIO | stable value, then rapid toggling | avoided calls, display latency |
| Mixed | RX, TX, GPIO, and 1 ms tick | maximum tick delay, CPU load |

The proposed thresholds are one input render per burst, no GPIO render for a stable value, no
lost or reordered bytes, and no increase in maximum tick delay. CPU time and refresh count must
decrease. A microsecond target should be set only after the reference measurement.

## Tests to add before refactoring

- a fake ncurses layer or injectable wrappers that count calls without a real terminal;
- table-driven tests for printable characters, three deletion forms, Enter, and F10;
- the 0, 62, 63, and 64-character boundaries and preservation of an unconsumed ready line;
- exact RX and TX byte order and output handling of `\r`;
- no GPIO render on repetition, plus a complete initial render;
- an integration run in a pseudo-terminal with resizing and a slow terminal.

## Verdict

The main problem is not the cost of a `waddch()` call. It is running ncurses and terminal I/O
inside sections that mask the scheduling signal. The second safest improvement is grouping
input bursts, followed by removing and grouping GPIO renders.

The recommended path is: **instrument, group renders, eliminate unchanged renders, then isolate
ncurses behind a single owner**. It preserves fixed buffers, the
`services -> sysCall -> HAL` boundaries, and the common USART contract while reducing terminal
calls and simulator jitter. Changes must be delivered separately from this audit.
