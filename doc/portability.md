## 🧱 Portability and Conditional Compilation Policy

TaskMate aims to be portable **without relying on the C preprocessor** for configuration.
Portability and configuration must be handled by:

- the directory structure (arch/, mcu/, board/)
- the build system (Makefile variables: ARCH, MCU, BOARD)
- the autoCode generator for system initialization.

This rule prevents:

- configuration spaghetti
- hard-to-read code paths
- and platform-specific hacks scattered all over the codebase.

**If you think you need an `#ifdef`, you are probably putting the code in the wrong place.**

---

## 🧩 Adding a New Hardware Target (ARCH / MCU / BOARD)

Thanks to the existing build and directory structure, adding support for
a new architecture, microcontroller, or board **does not require** changes to
the core system.


1️⃣ **New architecture (ARCH)**

Example: adding armv7m


- Create a new architecture directory: `src/hal/arch/armv7m/`

- Implement:
	- context switching,
	- atomic operations,
	- low-level interrupt handling.

- Extend the Makefile to accept: `ARCH=armv7m`

2️⃣ **New microcontroller (MCU)**

Example: adding stm32g474

- Create: `src/hal/mcu/stm32g474/`

- Implement MCU-specific peripherals:

	- timers, UART, GPIO, I²C, ADC, …

- Use the common HAL API headers from: `src/hal/api/`

- Extend the Makefile to map: `MCU=stm32g474`

3️⃣ **New board / platform (BOARD)**

Example: adding nucleoG474RE

- Create: `src/hal/board/nucleoG474RE/`

- Implement:
	- pin mapping (LEDs, buttons, debug UART),
	- board-specific initialization,
	- drivers_init.rc entries for drivers and services.

- Extend the Makefile to map: `BOARD=nucleoG474RE`

4️⃣ **Update drivers_init.rc file to set up and load the new drivers**

Once these directories and source files are in place, you can build TaskMate for the new target with:

`make ARCH=<arch> MCU=<mcu> BOARD=<board>`

**No changes to sysCore, no #ifdef, no hidden side effects.**
