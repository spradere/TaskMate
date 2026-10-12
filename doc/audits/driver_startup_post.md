# Driver startup POST audit

| Audit date | Version tag | HEAD |
| --- | --- | --- |
| 2026-10-12 | `v0.33` | `261e5db8c82a10704174a1d7e783f01a710cbbd2` |

## Question and scope

Which software and hardware checks can run when the `test1` drivers initialize or start, with a short normal power-on self-test (POST) and a more complete fail-safe diagnostic mode?

This source audit covers the five drivers declared by [test1_init.rc](../../srcs/user/target/test1/test1_init.rc#L9), their boot and run-level callers, and GPIO as a related HAL initialization path. `z600` has virtual USART and timer implementations, but no I2C, LCD, or RTC; it can test control flow and injected failures, not physical peripherals. No board measurements were made.

## Current startup and diagnostic limits

| Priority | Finding and evidence | Consequence for POST |
| --- | --- | --- |
| P1 | [Boot](../../srcs/system/boot.c#L32) checks the returned USART init/start states, then sends three progress markers without checking their write/send results. [Scheduler startup](../../srcs/system/sysCore/sys_scheduler.c#L61) ignores timer init, callback registration, and start results before loading the first context. | Timer failure has no controlled diagnostic handoff; a blocked USART transmit can prevent any report. |
| P1 | [Run-level startup](../../srcs/system/sysCall/sc_driver.c#L119) discards each init and start result and calls start even when init fails. [System startup](../../srcs/system/services/system.c#L119) only rechecks driver state, waits up to ten scheduling rounds, then halts. | The failing step and original cause are lost; startup has no explicit optional-device policy. |
| P1 | [I2C transactions](../../srcs/hal/mcu/atmega2560/at2560_i2c.c#L174) and [USART transmit](../../srcs/hal/mcu/atmega2560/at2560_usart.c#L204) poll peripheral flags without deadlines. [I2C scan](../../srcs/hal/mcu/atmega2560/at2560_i2c.c#L130) covers the bus and treats every non-ACK address status as an absent device. | An active POST or fail-safe probe can hang or mistake a bus fault for absence until bounded waits and distinct status handling exist. |
| P1 | [RTC init/start](../../srcs/hal/drivers/rtc/rtc_ZS042.c#L123) set status bits without a device transaction. [RTC read](../../srcs/hal/drivers/rtc/rtc_ZS042.c#L158) converts seven bytes without validating raw BCD, calendar fields, or device fault flags. [System startup](../../srcs/system/services/system.c#L149) ignores the startup-time read result. | A reachable but invalid RTC can be reported as ready, and later printed time may be invalid after a failed read. |
| P2 | The [driver state contract](../../srcs/interfaces/hal_drivers.h#L49) is bit based. USART, timer, and I2C init/start set bits after register writes; [LCD init](../../srcs/hal/drivers/lcd/lcd_AMC2004.c#L114) observes I2C ACKs but has no display readback. Operational errors can update `last_error` without setting the error bit, as in [I2C](../../srcs/hal/mcu/atmega2560/at2560_i2c.c#L63). | `RUNNING` and register readback prove software progress or peripheral register access, not external electrical function or display content. |
| P2 | [USART stop](../../srcs/hal/mcu/atmega2560/at2560_usart.c#L141) clears only the software start bit; its RX interrupt remains enabled, and init does not reset the ring indices. | A repeated or fail-safe start cannot assume quiescent hardware or empty buffers. |
| P2 | [GPIO signal initialization](../../srcs/hal/mcu/atmega2560/at2560_gpio.c#L77) wires and configures pins but returns no result. Output reads use the `PORT` latch rather than the `PIN` input register. | A software readback cannot detect an open trace, short, failed LED, or wrong physical wiring. |

The present [bus scan](../../srcs/system/sysCall/sc_driver.c#L224) runs after core driver startup and can mark configured LCD/RTC addresses `DEAD`. Its result is ignored by [system startup](../../srcs/system/services/system.c#L149); a full scan does more bus work than two targeted probes and has no deadline. It detects an ACK, not the identity or correctness of the responding device. An aborted scan also retains its current address because [reset occurs only at completion](../../srcs/hal/mcu/atmega2560/at2560_i2c.c#L155), so a diagnostic retry is not a fresh scan.

## Proposed normal POST

Run this once per cold boot in dependency order. Keep it bounded, read only except for existing startup commands, and preserve the exact failed operation and error before any later status query can replace `last_error`. Use target-specific HAL checks behind the existing interfaces; do not add diagnostic commands to the common `hal_*Control()` protocol.

| Stage | Fast check and success condition | What it can establish |
| --- | --- | --- |
| Static driver table | After allocation, verify the five expected control pointers, configured run levels, unique declared I2C addresses, and legal status transition `OFF -> INITIALIZED -> RUNNING`. Compare each returned state immediately; do not start after failed init. | Generated configuration and software lifecycle are coherent. This does not test hardware. |
| USART1, before scheduler | Check computed baud divisor fits UBRR1, required frame/enable bits read back, and RX/TX ring indices start empty and in range. Clear any stale `TXC1`, then bound a single existing progress-byte transmit by `UDRE1`/`TXC1` with a defined timeout. | MCU USART registers and transmit engine progress. It does not prove the board connector, line level, receiver, or baud accuracy. |
| Timer1 and scheduler | Check callback registration and timer init/start results; read back CTC, compare, interrupt-mask, and clock-select settings before first context load. Confirm the first scheduler callback within a bounded independent observation window, after the first context is valid. | Register setup and actual interrupt delivery. Register bits alone do not prove that scheduling or the oscillator works. |
| I2C controller | Check the configured bit rate/prescaler and TWI enable readback. With bounded bus waits, probe only the two configured addresses, `0x3c` and `0x68`, and distinguish ACK, NACK, bus error, and timeout. | Controller progress and device presence at expected addresses, not identity or payload correctness. |
| LCD | Keep current command sequence and its I2C ACK checks, including setup, clear, and entry mode. Require each command result and the I2C dependency to remain healthy. | Commands reached an ACKing device. There is no current read path to verify visible characters. |
| RTC | After I2C is ready, read the seven time registers once, validate each raw BCD digit and field, calendar consistency, and any oscillator/invalid-time flag supported by the confirmed RTC chip. Preserve the raw flags before masking them. | Communication and plausible time data. A plausible timestamp does not prove time is advancing. |
| GPIO wiring | Validate generated signal count, pin numbers, ports, and mode/pull configuration before use; read back DDR and PORT configuration for pins owned by `test1`. | Mapping and MCU register setup, not the external circuit. |

The target declares I2C at the core run level and LCD/RTC at the driver run level, so device probes must occur only after the I2C controller starts. USART and timer checks need dedicated boot handling because both are declared with run level `none` and start outside the run-level loop. The timer interrupt check must never run before the callback and initial thread context are ready.

## Proposed fail-safe diagnostic mode

Enter this mode by explicit maintenance selection or after a failed normal POST. Do not admit later run levels while a required driver is unverified, and skip dependent tests when their prerequisite fails. Give every operation a bounded deadline and an independent way to terminate; a timer under test cannot be its only timeout source. Record driver, phase, observed state, peripheral status, and first error in fixed storage. Use USART for reporting only after its own bounded check passes; otherwise use a board-specific indicator or external debugger.

| Driver or path | Extended check | Required fixture or limit |
| --- | --- | --- |
| USART1 | Exercise TX/RX with several bytes, ring wrap and overflow, and RX interrupt delivery; compare received sequence and report framing/overrun flags. | Physical loopback or a known peer is required. Do not silently echo test bytes into the operator console. |
| Timer1 | Observe several compare interrupts and software-counter advances; measure interval and jitter against an independent reference, then test stop/restart and absence of callbacks while stopped. | A second clock, logic analyzer, or watchdog is needed for frequency accuracy and a missing-interrupt deadline. Keep tests out of the naked ISR. |
| I2C | Probe expected addresses repeatedly, exercise START, repeated START, data read, STOP, and controlled recovery after NACK, stuck SDA/SCL, and timeout. A full address scan is optional and only after the bus is known responsive. | Fault injection or a bus fixture is needed for stuck lines and arbitration. A missing ACK alone must not be diagnosed as a broken controller. |
| RTC | Read two samples separated by an independently measured interval; check monotonic seconds across rollover, oscillator/invalid-time flags, and battery retention in a separate power-cycle procedure. | Confirm the fitted RTC chip and its register map first. Do not overwrite clock registers during routine POST. Power removal requires an external fixture. |
| LCD | Write a short diagnostic pattern to every row and visually confirm the display; optionally use optical feedback in a fixture. | ACKs cannot verify glass, backlight, contrast, or displayed pixels. This test is operator or fixture assisted and changes the display. |
| GPIO | Exercise only explicitly safe, dedicated pins through an external loopback or measured load; test both levels and input pull-ups. | Never drive unknown-connected pins or infer pin voltage from `PORT` latch readback. |

For a required core component such as the scheduler timer, a failed POST must prevent normal scheduling and reach a bounded halt or board-defined safe state. A failed optional LCD or RTC can be reported and skipped only if callers are taught to honor an unavailable result; the current system service reads and prints RTC data and writes LCD data without checking every returned error. Avoid a generic retry that clears `DEAD` or `ERROR` bits without rechecking the physical dependency.

## Resolution sequence and validation

1. Define the boot mode, required versus optional driver policy, maximum duration, independent timeout source, and fixed diagnostic record. Decide the exact RTC chip and which GPIO pins are safe to drive on the reference board.
2. Make USART, timer, and I2C polling bounded and propagate init/start and callback results through boot, scheduler, and run-level startup. Preserve the first failure and stop dependent startup. This is a prerequisite for either active test level.
3. Add the normal checks at each owning initialization stage, then add the extended checks behind an explicit mode or maintenance trigger. Keep chip-specific and electrical checks in the selected HAL or fixture, with only neutral results crossing the interface boundary.
4. Add host tests for success, each injected failure, init-before-start ordering, timeout, partial startup, dependency loss, RTC invalid bytes, and the required/optional policy. Existing [syscall driver tests](../../test/sysCall/test_sc_driver.c#L91) cover basic lifecycle result mapping but not run-level startup or hardware POST.
5. Build `test1`, measure added flash, static RAM, stack, and startup latency, and verify the worst-case timeout with faults injected. On the Arduino Mega, test normal boot, missing LCD/RTC, held I2C lines, disconnected USART peer, timer interrupt failure, and recovery. Use a fixture or instrument for electrical claims; a passing host test or build is insufficient.
