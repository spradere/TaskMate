# 🔧 Architecture Note — hal

## Historical developments
TaskMate began as AVR-centric code; `v0.21` separated architecture, MCU, and board ownership.
After `v0.28`, reusable drivers and target-owned GPIO wiring clarified hardware responsibilities.
From `v0.30`, neutral mechanism and driver contracts progressively moved to `interfaces/`.
Commits `f78057f`, `0ea843f`, and `9eb3b78` introduced neutral halt, atomic, and context contracts.
Commit `46c7521` made initial context restoration one indivisible AVR8 operation.
Commit `776edf5` physically removed `hal/public`; tag `v0.31` records that boundary.

## Current implementation
The implemented stack is `avr8 / atmega2560 / arduinoMega`, selected by `test1`. Architecture code
owns context, atomics, halt, string storage, compiler support, and memory reporting. MCU code
owns GPIO, I2C, USART, and the scheduling timers. Board code contributes hardware configuration.
Reusable drivers implement the AMC2004 LCD and ZS042 RTC contracts.

HAL operations are declared by neutral interfaces and implemented by selected target sources; no
public relay directory remains. A build guard rejects the old facade and an include checker prevents
system, tmLibc, interfaces, and tasks from reaching concrete architecture, MCU, or board headers.

The boot path initializes USART before allocating modules and starting the scheduler. Run-level
drivers are then initialized and started through syscalls. Context start restores the AVR stack,
registers, status, and interrupt return as one naked non-returning backend operation.

## Well-built code and implementation weaknesses
### Strengths
- Neutral contracts are separated from selected architecture, MCU, board, and driver code.
- Concrete register, context, interrupt, and timer details remain inside HAL implementation paths.
- Build guards reject facade regression and forbidden concrete HAL includes.
- Static selection and driver control keep hardware dispatch bounded and allocation-free.

### Remaining weaknesses
- Only one hardware stack validates the contracts and target-selection model.
- Target string macros are compiler-injected into portable consumers and expose storage semantics.
- USART and scheduling timers still follow special boot paths outside run-level startup.
- Driver-stage start discards individual results and cannot unwind partial hardware initialization.
