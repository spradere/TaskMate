## 🧱 Portability and Conditional Compilation Policy

Portability and configuration must be handled by:

- the directory structure (arch/, mcu/, board/)
- the build system (Makefile variables: ARCH, MCU, BOARD)
- autoCode generator for system initialisation.
- C preprocessor conditional statements

This rule prevents:

- configuration spaghetti
- hard-to-read code paths
- and platform-specific hacks scattered all over the codebase.

---

## 🌐 GPIO Architecture overview

---

### 1. Purpose of the GPIO subsystem

The GPIO subsystem provides a clean and portable abstraction for all inputs and outputs in TaskMate.
Its goals are:

- **Portability** across MCUs (AVR, STM32, AMD64 simulation, …)
- **Board-specific configuration** (each board assigns logical signals to physical pins)
- **Safety checks at runtime** (no unconfigured signal can be used)
- **Simple API for tasks** (gpioSignalSet(), gpioSignalGet())

The design follows a strict layered model :

| layer | classification |
|-------|----------------|
| Tasks	| Application layer |
| sysCall/gpio | Middleware / logical abstraction layer |
| HAL Board	| Configuration layer |
| HAL MCU | Hardware abstraction layer (strict) |
| MCU | registers	Hardware |

---

### 2. MCU HAL (hal/mcu/${MCU}/hal_gpio.c)

**Role**

This is the **lowest-level** GPIO driver, directly manipulating MCU registers.
It knows the physical ports of the micro-controller
 (`PORTA`, `DDRA`, `PINA` on AVR; `GPIOA->ODR`, `GPIOA->BSRR` on STM32).

**Responsibilities**

Provide raw operations:

- `halGpioWriteRaw(port_index, bit, value)`
- `halGpioReadRaw(port_index, bit)`
- `halGpioInitRaw(port_index, bit, mode, pull)`

Maintain an internal table describing the MCU ports:

```C
static const gpio_mcu_port_t gpioMcuPorts[] = {
    { &DDRA, &PORTA, &PINA },
    { &DDRB, &PORTB, &PINB },
    ...
    };
```

**Key point**

This layer does **not** know about “logical signals” or “board definitions”.
It works only with **physical** ports and bits.

---

### 3. Board Mapping (hal/board/${BOARD}/hal_boardInit.c)

**Role**

Defines how logical signals used by TaskMate are physically wired on a specific board.

**Responsibilities**

- Assign signals such as `GPIO_SIGNAL_LED_RUN` or `GPIO_SIGNAL_BTN_START` to MCU ports.
- Provide polarity (`active_high` or `active_low`).
- Call the sysCall functions to register these assignments at boot.

Example:

```C
sysGpioConfigureSignal(GPIO_SIGNAL_LED_RUN,
                       GPIO_PORT_A, 1,
                       true); // active high
```

**Key point**

This layer depends on the board hardware, not the MCU and not the tasks.

---

### 4. sysCall GPIO (sysCall/gpio.c)

**Role**

This layer implements the logical GPIO API used by tasks.
It enforces correctness and provides portability.

**Responsibilities**

- Hold the mapping logical signal → (port_index, bit, polarity).
- Validate that each signal has been configured before use.
- Apply active-high / active-low translation.
- Call into halGpioWriteRaw() and halGpioReadRaw().

Example:
```C
void gpioSignalSet(gpio_signal_t sig, bool on)
{
    const gpio_signal_cfg_t *c = sysGpioGetCfg(sig);
    bool hw = c->active_high ? on : !on;
    halGpioWriteRaw(c->port, c->bit, hw);
}
```

**Key point**

Tasks never manipulate raw ports.
They use only logical signals, which makes them fully portable.

---

## 🧩 Adding a New Hardware Target (ARCH / MCU / BOARD)

Thanks to the existing build and directory structure, adding support for
a new architecture, micro-controller or board **does not require** changes to
the core system.


1️⃣ **New architecture (ARCH)**

Example: adding arm32v7m


- Create a new architecture directory: `src/hal/arch/arm32v7m/`

- Implement:
	- context switching,
	- atomic operations,
	- low-level interrupt handling.
	- arch_init.rc entries for new drivers

- Use the common HAL API headers from: `hal/hal_api.h`
- Extend the Makefile to accept: `ARCH=arm32v7m`

---

2️⃣ **New micro-controller (MCU)**

Example: adding stm32g474

- Create: `src/hal/mcu/stm32g474/`

- Implement MCU-specific peripherals:

	- timers, USART, GPIO, I²C, ADC, …
	- mcu_init.rc entries for new drivers

- Use the common HAL API headers from: `hal/hal_api.h`
- Extend the Makefile to map: `MCU=stm32g474`

---

3️⃣ **New board / platform (BOARD)**

Example: adding nucleoG474RE

- Create: `src/hal/board/nucleoG474RE/`

- Implement:
	- pin mapping (LEDs, buttons, debug USART)
	- board-specific initialization
	- board_init.rc entries for new drivers

- Use the common HAL API headers from: `hal/hal_api.h`
- Extend the Makefile to map: `BOARD=nucleoG474RE`

---

4️⃣ Once these directories and source files are in place, you can build TaskMate for the new target with:

`make ARCH=<arch> MCU=<mcu> BOARD=<board>`

<span style="color:green">**No changes to core system, no `#ifdef` and no hidden side effects.**</span>
