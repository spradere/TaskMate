# 👨‍💻 autoCode: configuration-driven code generator

## 👀 overview

> 🚨 ⚠️ <span style="color:red"> autoCode is a critical component: if it generates incorrect code,
> TaskMate may still compile but will behave unpredictably at runtime.
> Any change to autoCode must be considered system-critical and tested accordingly.</span>

`autoCode` is a **code generation tool** used by TaskMate to automatically build
the initialisation code for drivers, services, and user tasks.
It consolidates all module definitions from `init.rc` configuration files
into the generated system files that TaskMate uses at runtime.

---

## ❓ How does it work?

`autoCode` is responsible for generating dense, low-level initialisation
code in files like `sysCore/modules.c`

This generated code is **not meant to be human-friendly**: it uses indexes,
tables, and “magic numbers” to keep the runtime overhead small and the layout explicit for the compiler.

Instead of relying on the readability of the generated code, TaskMate trusts the **generator** itself:

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

| Type          | Typical Path                          | Description                              |
|-------------- |----------------------------------------|------------------------------------------|
| configuration | `build/autoCode_config`				| hardware target, options, files path |
| global errors	| `build/errors_all.err`				| Global error catalog		|
| HAL headers	| `build/files_hal_user`				| List of hal user sources files |
| 				| `build/files_hal_system`				| List of hal system sources files |
| Drivers       | `hal/arch/<arch_name>/drivers_init.rc` | Hardware-dependent drivers |
| 				| `hal/mcu/<mcu_name>/drivers_init.rc` | |
| 				| `hal/board/<borad_name>/drivers_init.rc` | |
| Services      | `services/services_init.rc`        | System-level services  |
| User Tasks    | `tasks/tasks_init.rc`              | User-level or application tasks          |

The target architecture is usually passed from the Makefile,
and determines which folder under `hal/` is parsed.

---

## 📤 Output files

`autoCode` generates several key files that are later compiled into TaskMate:

| File                        | Role                                                                 |
|------------------------------|----------------------------------------------------------------------|
| `src/sysCall/auto_error.h`      					| Global error catalog header    |
|								 |															|
| `src/sysCore/runLevel.h`      					| run level header    |
| `src/sysCore/runLevel.c`      					| run level data    |
| `src/sysCall/error.c`      					| Global error catalog data    |
| `src/sysCall/systemCall.c`      					| Hardware target name arch/mcu/board    |
| `src/sysCore/modules_define.h`      					| Modules count header    |
| `src/sysCore/modules.c`      					| Modules data for drivers and threads    |
|												|										|
| `sysCore/auto_threads_list.h"`				| Thread list |
| `hal/auto_hal_system.h`						| List of hal headers, system type |
| `hal/auto_hal_user.h`						| List of hal headers, user type |
| `hal/auto_hal_define.h`						| List of hardware target `_define.h` |
| `hal/auto_hal_init.h`						| List of hardware target `_init.h`       |

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
- ✔️ Timestamped .autoCode_stamp file for Makefile dependency tracking
- 🔒 Only replaces generated files when all steps complete successfully and if the new file differs from the old one

Together, these safeguards ensure that autoCode behaves as a **transactional generator:**
either the entire configuration is validated and a consistent set of files is produced,
or no change is applied at all. This guarantees that build failures or partial updates
cannot leave the system in an inconsistent or undefined state.
