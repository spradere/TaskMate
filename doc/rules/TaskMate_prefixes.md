# 🧭 Code and file prefixes guidelines

## 🏷️ In TaskMate, prefixes are used deliberately and sparingly.

- Each prefix corresponds to a **clear conceptual responsibility**
- No prefix overlaps with another
- Source filenames use the prefix of the layer or target that owns their implementation

Symbol and filename prefixes are related, but they are not interchangeable. Public symbols
identify the API boundary they implement, whereas target-specific filenames identify where an
implementation is selected. For example, `avr8_context.c` implements the architecture-specific
part of the `hal_` API; its exported symbols therefore keep the `hal_` prefix.

---

## 📋 Overview of the existing prefixes and their intent.

### ● `tm_` — Project identity

**Everything that is specific to TaskMate itself**

- Acts as a global namespace
- Protects against external symbol collisions
- Serves as a mental anchor: “this code belongs to the TaskMate core”

👉 To be used **sparingly** and only when the project identity matters.

---

### ● `drv_` — Driver interface

**Portable control operations and state definitions shared by drivers**

- Names the constants declared by the neutral driver interface
- The corresponding generic interface headers use the `drv_<driver>.h` filename form
- Keeps the driver control protocol distinct from TaskMate core constants
- Uses `DRV_CTRL_` for every operation accepted by a driver control function

👉 This prefix is reserved for the **portable driver contract**, not for
hardware-specific implementation details.

---

### ● `ac_` — autoCode constants and enumerations

**Build-time code-generation definitions owned by autoCode**

- Identifies constants and enumeration values used by the autoCode tool
- Keeps build-time generator definitions distinct from generated TaskMate contracts
- Does not apply to generated symbols, which retain the prefix of their owning subsystem

👉 This prefix is reserved for definitions internal to the **autoCode generator**.

---

### ● `sc_` — Task-visible API boundary (system calls)

**Controlled and limited API access from tasks to system functionality**

- Exposes a restricted API to user tasks
- Clearly identifies calls that cross from tasks into system code
- Prefixes every C source and header owned by `srcs/system/sysCall/`

At a glance, it reads as:

> “This code is not the kernel, but it is talking to it.”

---

### ● `sys_` — sysCore implementation

**Kernel state, policy, boot support, and scheduling owned by sysCore**

- Prefixes every C source and header owned by `srcs/system/sysCore/`
- Distinguishes kernel implementation files from the task-visible `sc_` boundary
- Does not create a new public API namespace: symbols retain their established domain prefixes
  when appropriate

👉 `sys_` identifies **sysCore file ownership**, not generic project-wide code.

---

### ● `hal_` — Hardware abstraction boundary

**Target-independent HAL contracts and hardware-facing API symbols**

- Prefixes the public, target-selecting headers in `srcs/hal/public/`
- Keeps the portable hardware boundary distinct from a selected target implementation

👉 A `hal_*.h` filename exposes a **HAL capability**; target-specific implementation filenames use
the target prefixes below.

---

### ● `avr8_` / `at2560_` / `mega_` — Selected HAL implementation

**Files whose implementation belongs to a specific architecture, MCU, or board**

- `avr8_` prefixes files in `srcs/hal/arch/avr8/`
- `at2560_` prefixes files in `srcs/hal/mcu/atmega2560/`
- `mega_` prefixes files in `srcs/hal/board/arduinoMega/`
- Applies to implementation-support files as well as C and headers, including `.mk`, `.awk`,
  `.list`, and `.err` files
- Makes target selection explicit without scattering target conditionals through portable code

👉 Target prefixes identify **where an implementation belongs**, while `hal_` identifies the
hardware abstraction boundary it serves.

---

### ● `gpio_` — Logical-to-physical middleware

**An intermediate semantic layer between software logic and hardware signals**

- Not HAL
- Not application code
- Not a pure driver

👉 Using a dedicated prefix explicitly acknowledges the existence of an
**intermediate semantic layer**, rather than hiding it inside unrelated components.

---

### ● `err_` — Global error system

**Cross-cutting error handling infrastructure**

- Transversal by nature
- Ideal for auditing, logging, and diagnostics

Most importantly:

👉 A transversal concern **justifies** a transversal prefix.

---

### ● `mod_` — Module database

**System-wide description of threads and drivers**

- Provides a global view of the system
- Acts as a single source of truth
- Implements a quasi *“kernel metadata”* logic

Using a dedicated prefix avoids ambiguity:

👉 These symbols **describe the system as a whole**, not just one of its parts.

---

### ● `rl_` — Run levels

**System life cycle management**

- Small and well-defined domain
- Strong conceptual meaning
- Explicit life cycle logic

👉 A minimal prefix for a **fundamental OS primitive**.

---

## Summary and why this matters

A prefix is not decoration.

> **A prefix replaces a comment — it must not become noise.**

TaskMate prefixes provide:

- A clear taxonomy
- A short and controlled list
- Strong semantic meaning
- Real room for future evolution

▶️ **Prefixes name conceptual boundaries**
