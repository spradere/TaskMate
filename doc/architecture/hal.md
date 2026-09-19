# 🔧 Architecture Note — hal

## Historical developments
TaskMate began as AVR-centric code, then `v0.21` separated architecture, MCU, and board ownership.
This progressively removed hardware detail from sysCore and clarified target-specific boundaries.

After tag `v0.28`, the repository introduced reusable external drivers and moved target GPIO wiring.
Commit `b201809` completed the HAL/GPIO refactor; tag `v0.29` recorded the Make and HAL baseline.

Generic driver and mechanism contracts later moved to `interfaces/`; HAL retained implementation.

## Current implementation
The only implemented stack is `avr8 / atmega2560 / arduinoMega`, selected by `test1`:

- architecture code owns context, stack, interrupt, atomic, startup, and halt mechanisms;
- MCU code owns GPIO, I2C, USART, timers, startup, and AVR text/output support;
- board code provides the Arduino Mega startup hook;
- reusable drivers implement the AMC2004 LCD and ZS042 RTC contracts.

Neutral interfaces expose hardware-facing contracts, while the build selects the architecture,
MCU, board, and driver implementations. Source files include concrete HAL headers only within HAL
and target configuration. Before scheduling, generated calls initialize architecture, MCU,
board, then target hooks before logical GPIO initialization.
USART still starts earlier as the boot-log path. Once scheduled, the system service starts other
drivers by run level through syscalls and checks their running state before advancing.

LCD and USART output contracts transport `uint8_t` bytes. Their display paths are reserved for boot
or normal thread context and must never be called from an ISR. In particular, an LCD byte sequence
holds one I2C transaction from write start through write end; the syscall layer owns that sequencing
and finalizes a started transaction after either success or failure. Reentrancy is intentionally not
part of the contract until locking is added.

`tmLibc` reaches byte-oriented text and USART operations through `sysCall`. Build-selected AVR
string macros preserve program-memory access without exposing a public HAL facade.

## Well-built code and implementation weaknesses
### Strengths
- CPU context, interrupts, timers, and registers remain inside target-specific code.
- The build and neutral contracts reject incomplete target selections before compilation.
- The build-selected startup order is explicit, generated, and covered by host tests.
- Registered drivers share one bounded life-cycle and status contract.

### Remaining weaknesses
- Driver capability requirements remain implicit in selected sources and `init.rc` names.
- Build-injected AVR string macros still expose target storage policy to portable call sites.
- Startup hooks are empty; USART and scheduler timers still follow special initialization paths.
- Start requests discard driver results, and startup cannot unwind a partial hardware state.
