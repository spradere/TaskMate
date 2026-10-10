# Architecture Note: hal

## Historical developments
TaskMate began as AVR-centric code; `v0.21` separated architecture, MCU, and board ownership.
After `v0.28`, reusable drivers and target-owned GPIO wiring clarified hardware responsibilities.
Tag `v0.30` marks the migration of neutral mechanism and driver contracts into `interfaces/`.
Tag `v0.31` marks removal of the `hal/public` relay and an indivisible AVR initial context restore.
After `v0.32`, FreeBSD and ucontext moved under host-specific HAL directories and support the `z600` simulation target.
After `v0.32`, one timer-context driver replaced separate scheduling and software-counter timers.
After `v0.32`, AVR context assembly and timer lifecycle checks were hardened and a neutral delay API replaced driver-local delay macros.

## Current implementation
The physical stack is `avr8 / atmega2560 / arduinoMega`; the virtual stack is `ucontext / freebsd / pc`. Architecture or host code owns context, atomics, halt, strings, delays, and compiler support. MCU or host code owns GPIO, USART, and timer mechanisms. Reusable drivers implement the AMC2004 LCD and ZS042 RTC.

Neutral interfaces declare HAL operations. Selected sources implement them directly, and build checks prevent upper layers from including concrete architecture, MCU, board, or host headers.

Boot starts USART before static allocation. The scheduler initializes the timer-context driver, which provides both preemption and software-counter ticks. Generated driver capabilities let services compile for targets without optional I2C, RTC, LCD, or SCLI components.

## Well-built code and implementation weaknesses
### Strengths
- Neutral contracts are separated from selected architecture, MCU, board, host, and reusable-driver code.
- AVR register, interrupt, timer, and naked context details remain inside selected HAL sources.
- Physical and virtual targets exercise the same upper-layer contracts with static selection.
- Driver control stays limited to common lifecycle and status operations.

### Remaining weaknesses
- The ATmega2560 build is capped at 64 KiB of flash because context switching does not preserve the third program-counter byte.
- The virtual stack does not implement the complete AVR peripheral set, so peripheral portability remains weakly exercised.
- USART and timer-context startup remain special boot paths outside normal run-level driver startup.
- Run-level startup discards individual driver results and cannot unwind partial hardware initialization.
