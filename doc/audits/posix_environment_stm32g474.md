# POSIX compatibility environment on STM32G474 audit

Audit date: 2026-10-12

Version tag: `v0.33`

Analysed revision: `261e5db8c82a10704174a1d7e783f01a710cbbd2`

## Scope

This audit assesses whether TaskMate can host standard C programs and a basic POSIX-style environment on an STM32G474-class target. It covers a C runtime, file descriptors, a device namespace such as `/dev/fb0`, `/dev/pcm`, and `/dev/input/mice`, `ioctl()`, a useful `pthread` subset, and the process functions `fork()` and `exec()`.

The assessment is architectural. The STM32G474 target does not exist in the repository, so no ARM build or hardware test was performed. The prerequisite Cortex-M4F port is covered separately by the [FreeBSD/amd64 and STM32G4/Nucleo portability audit](portability_freebsd_stm32g4.md).

The device paths in the request are Unix or Linux conventions, not POSIX-standard device ABIs. A TaskMate implementation may use the names, but it must define its own stable request numbers, structures, blocking rules, and error behaviour unless compatibility with one specific external ABI is an explicit goal.

## Verdict

**A bounded, source-compatible embedded POSIX profile is feasible on STM32G474. A general POSIX process environment is not feasible without replacing the current TaskMate execution model.**

The recommended target is one statically linked firmware image containing:

- an embedded C library, preferably Picolibc or a similarly configurable library;
- a TaskMate OS adaptation layer below that library;
- one shared descriptor table and a small read-only `/dev` namespace;
- blocking `read()`, `write()`, `poll()`, clocks, and a documented `ioctl()` ABI;
- a bounded `pthread` subset backed by a statically sized pool of runtime thread records and stacks;
- optional device nodes selected by the target and implemented through sysCall and HAL;
- no `fork()`, no memory-mapped files, no dynamic linker, and no claim of complete POSIX conformance.

This profile can run many conventional C programs after recompilation if they use libc, file I/O, clocks, and basic pthread synchronization. It cannot run arbitrary Unix source unchanged when that source assumes processes, signals, terminals, sockets, a writable filesystem, virtual memory, or Linux-specific device contracts. It cannot run prebuilt Unix binaries.

## Feasibility by capability

| Capability | Verdict | Required model |
| --- | --- | --- |
| Pure ISO C functions | Feasible | Link an embedded libc and retain dead-code elimination |
| Hosted-style startup and `main()` | Feasible | Target CRT, `.data` and `.bss` setup, constructors if enabled, and defined `exit()` behaviour |
| `malloc()` family | Feasible with restrictions | Bounded heap, locking, failure tests, no ISR use, and an explicit real-time policy |
| `stdin`, `stdout`, `stderr` | Feasible | File descriptors 0, 1, and 2 mapped to a console device |
| `open`, `close`, `read`, `write`, `lseek`, `fstat` | Feasible subset | Descriptor and object tables, device operations, blocking states, and uniform errors |
| `/dev` and `ioctl()` | Feasible as a TaskMate ABI | Static device registry, validated request payloads, and target-selected backends |
| `pthread_create()` | Feasible bounded subset | Runtime thread allocation, wait states, join state, TLS, and synchronization primitives |
| Full pthread API | Not recommended | Cancellation, robust mutexes, process-shared objects, scheduling attributes, and signals greatly expand the kernel |
| `posix_spawn()` style program launch | Feasible only as an extension | Registry of statically linked entry points, or a bootloader handoff, with explicitly reduced semantics |
| `exec()` with POSIX semantics | Not feasible in the recommended model | Requires a process image, executable filesystem, loader, argument and environment setup, and descriptor inheritance |
| `fork()` with POSIX semantics | No-go | Requires an independently executing copy of the calling address space and process-owned resources |
| Arbitrary ELF binaries and shared libraries | No-go for the base profile | Requires an ABI, loader, relocations, executable storage, symbol resolution, and substantially more memory |

## Existing foundations

- The layer direction already supports a compatibility library above sysCall. Tasks may use `tmLibc` and sysCall without reaching HAL directly, while HAL remains below the neutral contracts.
- Context representation is opaque to sysCore through [`hal_context_t`](../../srcs/interfaces/hal_context.h#L24), which is suitable for a future Cortex-M implementation.
- The scheduler already owns round-robin policy and switches saved contexts through HAL in [`sys_scheduler.c`](../../srcs/system/sysCore/sys_scheduler.c#L106).
- Threads have generated records, target-configured stacks, canaries, high-water measurement, and a current-thread identity in [`sys_threads.c`](../../srcs/system/sysCore/sys_threads.c#L32).
- Short kernel operations use an interrupt-state token through [`hal_atomicStart()` and `hal_atomicEnd()`](../../srcs/interfaces/hal_atomic.h#L29).
- Drivers already separate common life-cycle control from driver-specific operations. That is compatible with device nodes dispatching normal data operations through dedicated syscalls.
- The FreeBSD target can become the functional test platform for descriptors, libc adaptation, and pthread semantics before Cortex-M hardware validation.

## Blocking findings

| Priority | Finding | Consequence | Required resolution |
| --- | --- | --- | --- |
| P0 | No STM32G474 or ARMv7E-M target exists | There is no startup, linker layout, exception context switch, timer, atomic implementation, or STM32 peripheral backend on which to host the environment | Complete and qualify the Cortex-M4F port before treating any POSIX layer as deployable |
| P0 | Every thread is generated at build time and stored in one fixed array | `pthread_create()` cannot allocate a record, choose a stack, pass an argument, collect a return value, or recycle a terminated thread | Add a bounded runtime TCB pool and stack policy while retaining generated system threads |
| P0 | The scheduler treats every non-zero admitted run level as runnable | A stopped, dead, sleeping, mutex-blocked, I/O-blocked, or joining thread can still be selected | Introduce explicit runnable and wait states, wait queues, wake reasons, and scheduler invariants |
| P0 | No process or address-space object exists | `fork()`, POSIX `exec()`, per-process descriptors, environment, signals, credentials, and current directory have no owner | Exclude process APIs from the embedded profile, or start a separately scoped process-kernel redesign |
| P0 | The syscall layer is an ordinary C boundary with no privilege or memory isolation | User pointers and `ioctl()` payloads cannot be made safe against faulty or hostile programs | State that the profile is cooperative and trusted, or add an MPU-based privilege model as a separate security project |
| P0 | Current TaskMate formatting has shared static state | Concurrent stdio calls can corrupt state, and `errno`, streams, locale data, and allocator state need per-thread or locked handling | Use a thread-safe embedded libc and supply TLS and lock hooks before enabling pthreads |
| P1 | The context initializer accepts only `void (*)(void)` and a stack top | Runtime threads need a stack base and size, alignment checks, an argument, and a return trampoline | Evolve the HAL context contract and autoCode together, then prove AVR and host non-regression |
| P1 | There is no descriptor, namespace, filesystem, or generic I/O object | Standard stdio and device paths cannot be expressed through the current driver-name and dedicated-operation APIs | Add a bounded descriptor core and static devfs with explicit object operations |
| P1 | Yield and delay are polling mechanisms | Busy waiting cannot implement efficient blocking I/O, mutexes, condition variables, joins, or timed waits | Add atomic block-and-switch and wake operations driven by ISR-safe event publication |
| P1 | No heap, TLS, or per-thread libc context is owned by the kernel | General libc and pthread code is not reentrant and cannot expose correct `errno` | Define heap ownership, TLS storage, libc reentrancy hooks, and teardown order |
| P1 | Device contracts are synchronous and specialized | Framebuffer, audio, and input need partial transfers, readiness, DMA completion, and blocking or non-blocking modes | Add stream-oriented HAL operations and event notification without expanding common driver control commands |
| P1 | Build selection only switches between TaskMate formatting and the host libc | An embedded libc needs headers, libraries, CRT objects, linker rules, syscall stubs, configuration, and license tracking | Add a target-selected libc integration path and reproducible configuration manifest |
| P2 | Time consists of a 16-bit per-thread decrementing counter | POSIX absolute clocks, timeouts, sleep, and monotonic timestamps cannot be implemented correctly | Add a monotonic system clock with a defined width, resolution, wrap policy, and timer wait queue |
| P2 | Existing public errors are TaskMate catalogue values | Standard functions require stable `errno` mappings and distinguish conditions such as `EAGAIN`, `EBADF`, `EINTR`, and `ENOTTY` | Define one translation boundary and test every public failure path |

## Current execution model versus pthreads

[`mod_threadsAlloc()`](../../srcs/system/sysCore/sys_threads.c#L145) populates records and contexts generated from `init.rc`. [`tagWriters_threads.c`](../../srcs/autoCode/tagWriters/tagWriters_threads.c#L27) emits one static stack for every declared thread. This is a strong deterministic model, but it is not a runtime thread-creation API.

The thread status word contains initialized, dead, and yielded flags, but [`tm_schedulerSelectNext()`](../../srcs/system/sysCore/sys_scheduler.c#L146) checks only the run level. [`sc_coopYield()`](../../srcs/system/sysCall/sc_threads.c#L173) requests a switch and then spins until selected again. These semantics must be corrected before they are reused for pthread blocking.

A suitable first pthread profile is:

- `pthread_create`, `pthread_exit`, `pthread_join`, `pthread_detach`, `pthread_self`, and `pthread_equal`;
- normal mutexes, condition variables, `pthread_once`, and a bounded number of TLS keys;
- static initializers where their representation can remain stable;
- stack size selection from a small set of target-defined pools;
- no asynchronous cancellation, robust mutexes, process-shared objects, priority protocols, or user-selected scheduling policy in the first version.

The kernel work should not make thread allocation unbounded. A configured pool can preserve deterministic memory use while allowing runtime creation until `EAGAIN`. Each TCB needs lifecycle state, join ownership, return value, wait object, timeout, TLS base, libc context, and a generation counter so stale `pthread_t` values are rejected.

The scheduler needs at least `RUNNABLE`, `SLEEPING`, `WAIT_OBJECT`, and `TERMINATED` states. Blocking must atomically enqueue the current thread and schedule another one. Wake-up from an ISR must perform a bounded state transition and defer complex work. Mutex and condition-variable operations must never hold a global interrupt mask while waiting.

## C runtime strategy

Implementing a general C library inside `tmLibc` is not recommended. The current layer intentionally provides a small subset, and its [`printf` and `snprintf` mapping](../../srcs/tmLibc/stdio/tm_snprintf.h#L34) does not provide standard stream semantics. Its formatter also stores all active state in one static object in [`tm_snprintf.c`](../../srcs/tmLibc/stdio/tm_snprintf.c#L46).

Picolibc is the strongest initial candidate because it is designed for statically linked embedded systems and documents the OS functions, TLS, locking, stdio, and heap integration points. Newlib is also viable, but its reentrancy structure and syscall veneers must be switched on every TaskMate context change and measured carefully. The choice must be made by a dedicated integration spike with flash, RAM, stack, and concurrency measurements.

TaskMate should support two mutually exclusive target profiles:

1. `tmLibc` for small, fixed firmware that values minimal code and RAM;
2. an embedded libc profile for conventional C source and the POSIX adaptation layer.

The embedded libc profile must define:

- startup ownership, initialization arrays, and what returning from `main()` means;
- per-thread `errno` and other libc state;
- libc lock hooks implemented by TaskMate mutexes;
- standard streams backed by descriptors 0, 1, and 2;
- heap bounds supplied by the linker, allocator locking, and deterministic failure behaviour;
- clock, calendar, random, termination, and assertion hooks;
- supported locale, multibyte, floating-point formatting, and wide-character features;
- an explicit ban on libc and allocation calls from interrupt context unless individually proved safe.

`malloc()` can be offered, but it weakens time and fragmentation guarantees. Safety-critical or latency-sensitive code should use fixed pools or caller-owned storage. The system must return allocation failure rather than grow a heap into stacks, and tests must cover exhaustion, fragmentation, concurrent allocation, and teardown.

## Descriptor and device model

The smallest useful model is one process-like container shared by all pthreads. Its descriptor table contains fixed slots referencing system-wide open objects. Each open object contains access flags, offset where meaningful, driver-private state, reference count, wait channels, and a pointer to constant operations.

Recommended operations are `open`, `close`, `read`, `write`, `lseek`, `fstat`, `ioctl`, and `poll`. Directory traversal, mounts, writable regular files, sockets, pipes, `mmap`, ownership, and permissions should remain outside the first profile.

The namespace can be a constant target-generated registry rather than a general VFS:

```text
/dev/console
/dev/null
/dev/zero
/dev/fb0       optional
/dev/pcm       optional
/dev/input/mice optional
```

The POSIX-facing library belongs above sysCall. Descriptor policy and wait state belong in sysCore. Device-independent requests pass through focused syscalls. Hardware and DMA remain in HAL. The intended path is:

```text
application -> embedded libc / POSIX compatibility -> sysCall -> sysCore or HAL -> hardware
```

Do not place VFS or pthread behaviour in `interfaces/`. That directory is dependency-neutral and should contain only portable contracts needed across the established layers. Do not route device-specific operations through `hal_<driver>Control()`. The common control API must remain limited to lifecycle, run level, status, state, and last-error access.

The public `ioctl(int fd, unsigned long request, ...)` veneer may be variadic for source compatibility, but the internal syscall should receive a validated request descriptor containing a fixed-width request number, direction, expected payload size, and pointer. Request encodings must not depend on native enum or pointer size. Unknown requests return `ENOTTY`.

Blocking I/O must define partial-transfer behaviour, interruption policy, non-blocking `EAGAIN`, timeout clock, close-versus-wait races, and wake-up from DMA or peripheral ISRs. Copying and state changes must be bounded while interrupts are masked.

## Requested devices

### `/dev/fb0`

This is feasible only for a selected display and memory arrangement. The STM32G474RE has 128 KiB of internal SRAM. A 320 by 240 RGB565 framebuffer alone needs 153,600 bytes, so it cannot reside in internal SRAM with the kernel and thread stacks. A 160 by 128 RGB565 buffer needs 40,960 bytes and may fit after a complete memory budget. Larger displays need external memory, display-controller RAM, tiles, scan lines, or a command-oriented device instead of a true framebuffer.

The first ABI should support information and mode queries plus `read()` and `write()` of a defined pixel format. `mmap()` should be excluded. Flush, dirty rectangles, buffer ownership, DMA completion, cache rules, and concurrent writers must be explicit.

### `/dev/pcm`

PCM playback or capture is feasible. The STM32G474 family provides DAC channels, I2S-capable SPI instances, SAI, and DMA. A practical driver needs fixed DMA ring buffers, sample format and rate negotiation, underrun and overrun reporting, blocking and non-blocking transfer semantics, and exclusive or deliberately mixed access.

The device name and ioctls would be TaskMate-specific unless a particular OSS, ALSA, or other ABI is deliberately selected. Claiming generic Unix compatibility for a custom `/dev/pcm` contract would be misleading.

### `/dev/input/mice`

An event stream is feasible, but the physical input source is board-dependent. The STM32G474 product specifies a USB device controller, not a USB host controller, so it cannot directly host a normal USB mouse without an external host controller or a different MCU. PS/2, SPI, I2C, UART, or a board-specific pointing device can feed the same upper device node.

If the Linux `/dev/input/mice` byte protocol is copied, document that exact protocol and its limitations. A typed TaskMate input-event structure is easier to extend and safer across architectures, but it provides only source compatibility through a dedicated adapter.

## Why `fork()` is outside the target

POSIX `fork()` creates a child that can execute independently and initially observes a copy of the parent's address space and process resources. TaskMate has one linked image, one global address space, static module databases, and no process resource owner. The STM32G474 MPU can protect regions, but it is not an MMU and provides neither virtual address spaces nor copy-on-write.

A literal memory copy is not a practical substitute. It would require relocatable process regions, duplicated writable data and stacks, copied descriptor references, allocator and lock repair, bounded stop-the-world latency, and enough spare RAM for the child. With 128 KiB shared by the kernel, drivers, buffers, stacks, libc, and applications, this design has little useful capacity and poor real-time behaviour.

Do not provide a function named `fork()` that creates a thread or returns `ENOSYS` only after partially changing state. Either omit the feature through the profile feature-test macros or provide a deterministic `-1` and `ENOSYS` compatibility stub only when third-party configuration probes require the symbol.

## `exec()` and program launch alternatives

POSIX `exec()` replaces the current process image from an executable file and preserves defined process resources. There is no current process image to replace, no executable filesystem, and no loader. A conforming implementation therefore has the same architectural dependency as processes.

Three narrower mechanisms are possible:

1. A generated program registry maps a path to a statically linked entry function and creates a fresh bounded application container. This is useful, but it is not general `exec()` and cannot provide isolation from global variables in the shared image.
2. A bootloader validates and starts a complete firmware image from another flash slot. This gives strong image replacement semantics only across a reset and does not preserve descriptors or pthread state.
3. An ELF loader reads position-independent images from external flash and assigns fixed MPU regions. This can provide isolated applications, but it needs an ABI, relocations, executable storage, process ownership, fault handling, and a much larger kernel. It should be a separate project, not part of the base POSIX layer.

For the requested use case, a documented `posix_spawn()`-like registry is the only proportionate option. It should use a TaskMate name until its return, inheritance, file-action, and error semantics satisfy the chosen POSIX profile.

## Recommended architecture sequence

1. Freeze a feature manifest that lists every supported header, function, option, error, and deliberate omission. Use the term "TaskMate POSIX compatibility profile", not "POSIX compliant".
2. Complete the STM32G474 Cortex-M4F port, including startup, linker script, PendSV context switching, timer, atomics, stack sizing, USART, and hardware tests.
3. Evolve the context and thread records for stack base and size, argument entry, return trampoline, TLS pointer, and lifecycle state. Update autoCode and all existing backends together.
4. Add scheduler blocking, timeouts, wait queues, and ISR-safe wake-up. Validate races on the deterministic FreeBSD target before hardware stress tests.
5. Implement mutexes, condition variables, joins, and the bounded runtime TCB and stack pools. Expose only the selected pthread subset.
6. Integrate one embedded libc with per-thread state and locks. First validate pure libc, startup, `errno`, heap exhaustion, and concurrent formatting without device files.
7. Add the bounded descriptor core and static devfs. Bring up `/dev/null`, `/dev/zero`, and `/dev/console`, then connect standard streams.
8. Add monotonic clock, sleeps, timed waits, `poll()`, and non-blocking I/O.
9. Add framebuffer, PCM, and input devices one at a time for a fixed board, with DMA and memory budgets measured for each target configuration.
10. Add a separately named static program registry only if multiple application entry points remain necessary. Keep `fork()` and conforming `exec()` unsupported.

## Validation requirements

### Kernel and pthread

- prove context preservation, FPU policy, stack alignment, task return, join, detach, pool exhaustion, ID reuse, and TLS isolation;
- exercise block-versus-wake, timeout-versus-event, close-versus-I/O, and interrupt-versus-thread races;
- verify that no non-runnable or terminated thread is selected and that every wait can be cancelled during object teardown;
- measure interrupt latency, context-switch time, maximum critical sections, stack high-water marks, and worst-case wake latency on hardware.

### Libc and descriptors

- run the selected libc test suites for enabled features on FreeBSD and the ARM target;
- test `errno`, streams, allocator locks, partial I/O, `EAGAIN`, invalid descriptors, unknown ioctls, and descriptor exhaustion concurrently;
- verify the ELF and map files against fixed flash, RAM, heap, stack, DMA, and framebuffer budgets;
- prove that unsupported functions are absent or fail exactly as the feature manifest specifies.

### Devices

- use DMA guard regions and canaries around every ring or framebuffer;
- test audio underrun and overrun, display transfer failure, input bursts, device removal where applicable, and restart after errors;
- validate rates and latency with external measurements, not scheduler tick assumptions;
- confirm that each optional device is absent from the namespace and binary when its target capability is disabled.

## Exit criteria for a first useful profile

- STM32G474 boots and preempts reliably with measured stack and timing margins;
- at least eight runtime-created threads repeatedly create, synchronize, join, and recycle within fixed pools;
- per-thread `errno` and libc state remain isolated under forced preemption;
- `stdin`, `stdout`, and `stderr` operate through descriptors and `/dev/console`;
- `open`, `close`, `read`, `write`, `fstat`, `ioctl`, `poll`, monotonic time, and sleep pass normal, exhaustion, and concurrency tests;
- one DMA-backed stream device runs under simultaneous console and pthread load without data corruption;
- the published feature manifest explicitly excludes `fork()`, POSIX `exec()`, `mmap()`, process signals, and arbitrary binary loading;
- the reference `test1` build, autoCode tests, boundary checks, and FreeBSD tests remain unchanged or pass with intentional target-selected differences.

## Effort and risk

Excluding the prerequisite STM32 port and device-specific hardware work, libc plus console descriptors is a medium project. A robust descriptor core, blocking scheduler, and useful pthread subset is a large kernel evolution. The dominant risks are concurrency correctness, libc reentrancy, bounded memory use, and stable behaviour under I/O interrupts.

Adding one framebuffer, one audio path, and one pointer-input backend is separate board work. The cost depends more on the display, codec, input transport, DMA plan, and external memory than on the pathname API.

A process model with ELF loading is a new operating-system architecture. It should not be scheduled as an extension of the compatibility layer, and `fork()` should remain rejected for this MCU class.

## External references

- [The Open Group POSIX.1-2024 `fork()` definition](https://pubs.opengroup.org/onlinepubs/9799919799/functions/fork.html)
- [The Open Group POSIX.1-2024 `exec` definition](https://pubs.opengroup.org/onlinepubs/9799919799/functions/exec.html)
- [Picolibc operating-system integration](https://github.com/picolibc/picolibc/blob/main/doc/os.md)
- [Picolibc embedded-system usage](https://github.com/picolibc/picolibc/blob/main/doc/using.md)
- [Newlib system calls and reentrancy](https://sourceware.org/newlib/libc.html)
- [STMicroelectronics STM32G474RE product information](https://www.st.com/en/microcontrollers-microprocessors/stm32g474re.html)
- [STMicroelectronics STM32G474xB/xC/xE datasheet](https://www.st.com/resource/en/datasheet/stm32g474rc.pdf)
