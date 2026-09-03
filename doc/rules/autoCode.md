# 👨‍💻 autoCode: configuration-driven code generator

## 👀 overview

> 🚨 ⚠️ <span style="color:red"> autoCode is a critical component: if it generates incorrect code,
> TaskMate may still compile but will behave unpredictably at runtime.
> Any change to autoCode must be considered system-critical and tested accordingly.</span>

`autoCode` is a **code generation tool** used by TaskMate to automatically build
the initialisation code at compile time for drivers, services, and user tasks.
It consolidates all module definitions from `init.rc` configuration files
into the generated system files that TaskMate uses at runtime.

---

## ❓ How does it work?

`autoCode` is responsible for generating dense, low-level initialisation
code in files like `sysCore/modules_list.h`

- autoCode parses architecture-specific and generic init.rc files (drivers, services, tasks).
- It validates tags, run levels and module definitions.
- It prints a detailed, human-readable summary of all discovered modules and their run-level mapping.

In other words, autoCode acts both as a **configuration validator** and as
a **single source of truth** for system initialisation. As long as autoCode
is correct and the `init.rc` files are consistent, the generated code can remain
dense and optimised without impacting maintainability.

---

## 📁 Input Files

`autoCode` reads initialisation files describing which modules are active
for the target architecture and how they should be initialised.

Each module line contains two required command/data pairs:

```text
<module> -type <driver|service|user> -run <none|core|driver|service|user>
```

The `-type` and `-run` pairs may appear in either order, but each command is required exactly once.
Drivers may also declare one optional scanned I2C address (`0x00` to `0x7E`) with
`-i2c <address>`. The parser accepts decimal or `0x` hexadecimal notation and rejects this option for
services and user tasks.

| Type          | Typical Path                          | Description                              |
|-------------- |----------------------------------------|------------------------------------------|
| configuration | `build/autoCode_config`				| hardware target, options, files path |
| global errors	| `build/errors_all.err`				| Global error catalogue		|
| Drivers       | `hal/arch/<arch_name>/drivers_init.rc` | Hardware-dependent drivers |
| 				| `hal/mcu/<mcu_name>/drivers_init.rc` | |
| 				| `hal/board/<board_name>/drivers_init.rc` | |
| Services      | `services/services_init.rc`        | System-level services  |
| User Tasks    | `tasks/tasks_init.rc`              | User-level or application tasks          |

The target architecture is usually passed from the Makefile,
and determines which folder under `hal/` is parsed.

---

## 📤 Output files

`autoCode` generates several key files that are later compiled into TaskMate:

| File                        | Role                                                                 |
|------------------------------|----------------------------------------------------------------------|
| `interfaces/error_catalog.h`    					| Global error catalogue header    |
| `sysCall/error.c`      					| Global error catalogue data    |
|								 |															|
| `sysCore/runLevel.h`      					| run level header    |
| `sysCore/runLevel.c`      					| run level data    |
| `sysCore/modules_list.h"`				| all modules list |
| `sysCore/modules_define.h`      					| Modules count header    |
| `sysCore/modules.c`      					| Modules data for drivers and threads    |
| `sysCore/hal_init.h`						| List of hardware target `_init.h`       |
|												|										|
| `hal/public/sysInfo.c`      					| Hardware target name arch/mcu/board    |
| `hal/public/define.h`						| List of hardware target `_define.h` |


---

## 📝 Logging

During execution, autoCode produces a detailed, human-readable log summarising:

- all parsed init.rc files.
- detected modules and their assigned run levels.
- generated source and header files.
- and any validation or consistency errors.

This output is intended for human verification, not for machine parsing, and serves
as the primary diagnostic tool when modifying system configuration or autoCode itself.

---

## 🧰 Reliability features

- ✔️ Internal validation of tags, indexes and duplicate entries
- ✔️ Detailed logging with file and line number
- ✔️ Timestamped `.autoCode_stamp` file for Makefile dependency tracking
- 🔒 Replaces generated files only when all steps complete successfully and the new file differs from the old one

Together, these safeguards ensure that autoCode behaves as a **transactional generator:**
either the entire configuration is validated and a consistent set of files is produced,
or no change is applied at all. This guarantees that build failures or partial updates
cannot leave the system in an inconsistent or undefined state.
