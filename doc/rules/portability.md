# 🧱 Portability and hardware abstraction design


This document describes the architectural rules and design principles
governing portability in TaskMate. It is intended as a technical guide
rather than an introductory overview.

**Portability and configuration are handled by:**

- the directory structure (arch/, mcu/, board/)
- the build system (Makefile variables: ARCH, MCU, BOARD)
- autoCode generator for system initialisation.
- C preprocessor conditionals

These rules prevent:

- configuration spaghetti
- hard-to-read code paths
- and platform-specific hacks scattered all over the codebase.

---

## 🌐 GPIO Architecture overview

### 1. Purpose of the GPIO subsystem

The GPIO subsystem provides a clean and portable abstraction for all inputs and outputs in TaskMate.
Its goals are:

- **Portability** across MCUs (AVR, STM32, AMD64 simulation, …)
- **Board-specific configuration** (each board assigns logical signals to physical pins)
- **Safety checks at runtime** (no unconfigured signal can be used)
- **Simple API for tasks** (gpioSignalSet(), gpioSignalGet())

The design follows a strict layered model:

| layer | classification |
|-------|----------------|
| Tasks	| Application layer |
| sysCall/gpio | Middleware / logical abstraction layer |
| HAL Board	| Configuration layer |
| HAL MCU | Hardware abstraction layer (strict) |
| MCU | Hardware register |

---

### 2. MCU HAL (hal/mcu/${MCU}/hal_gpio.c)

**Role**

This is the **lowest-level** GPIO driver, directly manipulating MCU registers.
It knows the physical ports of the microcontroller
 (`PORTA`, `DDRA`, `PINA` on AVR; `GPIOA->ODR`, `GPIOA->BSRR` on STM32).

**Responsibilities**

Provide raw operations:

```C
void hal_gpioInitPin(const gpio_pin_item_t *pin);
void hal_gpioWritePin(const gpio_pin_item_t *pin, bool value);
bool hal_gpioReadPin(const gpio_pin_item_t *pin);
```

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

### 3. Board mapping (hal/board/${BOARD}/boardInit.c)

**Role**

Defines how logical signals used by TaskMate are physically wired on a specific board.

**Responsibilities**

- Assign signals such as `GPIO_SIGNAL_LED_RUN` or `GPIO_SIGNAL_BTN_START` to MCU ports.
- Provide polarity (`active_high` or `active_low`).
- Call the sysCall functions to register these assignments at boot.

Example:

```C
	// gpio pin definition
	gpio_signal_item_t sig;

	sig.pin.port_index = GPIO_PORT_B;
	sig.pin.number = PB7;
	sig.pin.mode = GPIO_PIN_MODE_OUTPUT_PP;
	sig.pin.pull = GPIO_PIN_PULL_NONE;
	sig.active_high = true;
	gpio_signalInit(GPIO_SIGNAL_INBOARD_LED, &sig);
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

Example:
```C
void gpio_signalSet(gpio_signal_t signal, bool on)
{
	bool val = gpio_signals_table[signal].active_high ? on : !on;
	hal_gpioWritePin(&(gpio_signals_table[signal].pin), val);
}
```

**Key point**

Tasks never manipulate raw ports. They use only logical signals,
which makes them fully portable.

---

## 🧩 Adding a new hardware target (ARCH / MCU / BOARD)


> TODO add hal/public


1️⃣ **New architecture (ARCH)**

Example: adding arm32v7m

- Create a new architecture directory: `src/hal/arch/arm32v7m/`

- Implement:
	- context switching,
	- atomic operations,
	- low-level interrupt handling.
	- arch_init.rc entries for new drivers

- Extend the Makefile to accept: `ARCH=arm32v7m`

---

2️⃣ **New micro-controller (MCU)**

Example: adding stm32g474

- Create: `src/hal/mcu/stm32g474/`

- Implement MCU-specific peripherals:

	- timers, USART, GPIO, I²C, ADC, …
	- mcu_init.rc entries for new drivers

- Extend the Makefile to map: `MCU=stm32g474`

---

3️⃣ **New board / platform (BOARD)**

Example: adding nucleoG474RE

- Create: `src/hal/board/nucleoG474RE/`

- Implement:
	- pin mapping (LEDs, buttons, debug USART)
	- board-specific initialization
	- board_init.rc entries for new drivers

- Extend the Makefile to map: `BOARD=nucleoG474RE`

---

4️⃣ Once these directories and source files are in place, you can build TaskMate for the new target with:

`make ARCH=<arch> MCU=<mcu> BOARD=<board>`

<span style="color:green">**No changes to core system, no hidden side effects.**</span>
