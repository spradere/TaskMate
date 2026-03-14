# TaskMate RTOS ![](doc/icon_64.png)

**Microcontroller Unit - Real-Time Operating System**

---

## ▶️ Introduction

**TaskMate** is a lightweight, preemptive real-time operating system.
Designed specifically for **micro-controllers**.
It emphasises **reliability** and **modularity**.
Without relying on any external RTOS — everything is built entirely from scratch.

**TaskMate** is structured around a clean and portable architecture designed
to separate build logic, system logic, and hardware dependencies.

> <span style="color:green"> **Project Stats (v0.26 [^1] )**</span>
>
>  <span style="color:green">380 commits • 106 source files • 6818 lines of code •
> binary size : 6496 bytes (Flash) • ram usage : 2009 bytes</span>

> ⚠️ <span style="color:red">**Development Status**</span>
>
> <span style="color:red">TaskMate is currently in active development and
> should be considered **experimental**. While the core system and architecture are
> functional, many components are still evolving. It is **not yet suitable for production use**,
> and both API and internal structures may change without notice.</span>


---

## 🧭 About ChatGPT and TaskMate

Although **no code from ChatGPT is ever copied directly** into the TaskMate source
tree, the project would never have reached its current level of maturity without
the assistance of AI. ChatGPT has been an invaluable tool for **structuring ideas,
learning new concepts, and refining both code and architectural design.** It
provides **technical guidance**. Moreover, it enables **efficient
research** on related topics by summarising and contextualising complex technical
information, helping me focus on building rather than endlessly searching.

See : [The Story of TaskMate and the AI Companion](doc/the_AI_companion.md)

---

## ⚙️ Build System

TaskMate uses a custom **Build system** that fully manages dependencies and workflow.

- Automatic recompilation based on file changes, including headers and configuration files.
- CLI commands like `make upload`, `make push` and `make backup`.

**Portability relies mostly on build-time source selection, with minimal use of preprocessor logic.**

![Build system](doc/build_v2.png)

---

## 🧱 Hardware Abstraction Layer and architecture support

The HAL provides a **clean interface** between the system and the hardware.
Ensures true portability across hardware families.
This allows TaskMate to run on multiple architectures:

- **avr8** - the historical beginning of TaskMate
- **amd64** - for testing and faster development cycles
- **arm32v7-m4** - planned for future hardware performance upgrades

See : [Portability](doc/rules/portability.md)

---

## ⏱️ Real-Time Behaviour

Although TaskMate includes preemptive scheduling and a software real-time clock,
it **is not yet a true real-time operating system** in the strict sense.

At its current stage, TaskMate guarantees **task switching** and **time slicing** with good stability,
but it does not yet ensure **hard real-time determinism.**

System latency and jitter are acceptable for testing and lightweight applications,
yet they remain **non-deterministic** under specific conditions such as nested interrupts,
driver contention, or prolonged critical sections.

---

## ⬆️ Layers

![System Layer Diagram](doc/TaskMate_layers_v6.png)

TaskMate layer configuration provides a **strong isolation between system components**.
Each layer communicates through **well-defined interfaces**,
preventing direct access to the hardware or core system logic.

User tasks can still **benefit from all system features** —
such as messaging, timing, I/O, and services — but always through indirect calls.
This design significantly improves **stability** and **portability**.

---

## 🧩 Modular Design & autoCode

Starting with version 0.10, TaskMate uses a **modular design model**:

- Drivers, system services, and user tasks are placed in dedicated directories.
- Each directory provides an init.rc file describing initialisation parameters.
- These files are parsed at **compile time** by a custom tool: `autoCode`.

The result is **auto-generated code**, without runtime overhead 👍

This approach keeps the flexibility of a dynamic system but ensures that
 **everything is resolved at compile time**, minimising Flash and RAM usage.
This mechanism defines system initialisation **without manually hard-coding**
configuration.

See : [More about autoCode](doc/rules/autoCode.md)

---

## ️📜 License

This software is distributed under the **TaskMate License v1.0**.

- Free for **non-commercial use** under conditions described in the `LICENSE` file.
- Commercial use requires a **separate paid license**.

To inquire about commercial licensing, please open an issue in this repository:
[Open Licensing Issue](https://codeberg.org/Doul09/TaskMate/issues).
By using this software, you agree to the terms of the TaskMate License v1.0.
See the `LICENSE` file for full details.

---

## 📟  Hardware setup : avr8 - atmega2560 - Arduino mega board

![Arduino mega board](doc/hardware_mega.jpg)

---

## 📑 Documentation & books 📚

- **Project development status** — see [Project Progress](doc/progress.md)
- **Changelog** — version history: see [CHANGELOG](./CHANGELOG)
- **C Style Guide** — best practices (pointers, errors, etc.): see [code best practices](./doc/C_code_best_practices.md)

You will find more information about the architecture in the doc/architecture/files.
These files contain information about the development history, the current implementation,
the strengths and weaknesses of the source code, and future improvements.

- La référence du C norme ANSI-ISO, author Claude Delannoy, publisher Eyrolles. ISBN 2-212-09036-6
- Microcontrôleurs AVR : des ATtiny aux ATmega, author Christian Tavernier, publisher Dunod. ISBN 978-2-10-074417-6
- The markdown guide, author Matt Cone, publisher Amazon. ISBN 9798656504492
- Hands-On RTOS with Microcontrollers, author Brian Amos, publisher Packt. ISBN 978-1-83882-673-4
- Making Embedded Systems, author Elecia White, publisher O'Reilly. ISBN 978-1-098-15154-6
- Operating System Design, The Xinu Approach, third edition, author Douglas Comer, publisher CRC Press, ISBN 978-1-032-98099-7
- The design and implementation of the FreeBSD operating system, second edition, authors Marshall Kirk McKusick, George V. Neville-Niel and Robert N.M. Watson, publisher Addison-Wesley, ISBN 978-0-312-96897-2

[^1]: ⚠️ Warning : Versions 1.37, 2.71, 3.14 and 4.2 are intentionally skipped. Universe backward compatibility constraints apply.

