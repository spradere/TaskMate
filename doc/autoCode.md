# 👨‍💻 autoCode: configuration-driven code generator

## 👀 overview

> 🚨 ⚠️ <span style="color:red"> autoCode is a critical component: if it generates incorrect code,
> TaskMate may still compile but will behave unpredictably at runtime.
> Any change to autoCode must be considered system-critical and tested accordingly.</span>

`autoCode` is a **code generation tool** used by TaskMate to automatically build
the initialisation code for drivers, services, and user tasks.
It consolidates all module definitions from `*_init.rc` configuration files
into the generated system files that TaskMate uses at runtime.

---

## ❓ How it's work ?

`autoCode` is responsible for generating dense, low-level initialisation
code in files like `initSys.c` and a set of auto-generated headers (`autoInclude.h` & `autoAlloc.h`).

This generated code is **not meant to be human-friendly**: it uses indexes,
tables, and “magic numbers” to keep the runtime overhead small and the layout explicit for the compiler.

Instead of relying on the readability of the generated code, TaskMate trusts the **generator** itself:

- autoCode parses architecture-specific and generic *_init.rc files (drivers, services, tasks).
- It validates tags, run levels and module definitions.
- It prints a detailed, human-readable summary of all discovered modules and their run-level mapping.


In other words, autoCode acts both as a **configuration validator** and as
a **single source of truth** for system initialisation. As long as autoCode
is correct and the .rc files are consistent, the generated code can remain
dense and optimised without impacting maintainability.

---

## 📁 Input Files

`autoCode` reads `*_init.rc` initialisation files describing which modules are active
for the target architecture and how they should be initialised.

| Type        | Typical Path                          | Description                              |
|--------------|----------------------------------------|------------------------------------------|
| Drivers      | `src/hal/arch/<arch_name>/drivers_init.rc` | Hardware-dependent drivers (I2C, WART…)  |
| | `src/hal/mcu/<mcu_name>/drivers_init.rc` | |
| | `src/hal/board/<borad_name>/drivers_init.rc` | |
| Services     | `src/services/services_init.rc`        | System-level services (CLI, msg server)  |
| User Tasks   | `src/tasks/tasks_init.rc`              | User-level or application tasks          |

The target architecture is usually passed from the Makefile,
and determines which folder under `hal/` is parsed.

---

## 📤 Output Files

`autoCode` generates several key files that are later compiled into TaskMate:

| File                        | Role                                                                 |
|------------------------------|----------------------------------------------------------------------|
| `src/sysCore/initSys.c`      					| modules data base initialisation routines executed during system startup.    |
| `src/sysCore/runLevel.c`      				| run level initialisation routines executed during system startup.    |
| `src/sysCore/TaskMate.c`      				| write target information arch/mcu/board.    |
| `src/sysCall/error.c`      					| Global error catalog.    |
| `scr/sysCore/autoInclude_threads.h`			| Centralised header including all service/task.        |
| `scr/hal/autoInclude_hal_user.h`      		| Centralised header including user hal calls. |
| `scr/hal/autoInclude_hal_system_critical.h` 	| Centralised header including system only hal calls. |
| `src/sysCore/include/autoAlloc.h`     		| Static allocation tables for modules.                                |

---

## 📝 Logging Example

A typical autoCode run looks like this:

```
[autoCode.c] info : target avr8 -> atmega2560 -> arduino_mega
[globalError.c] info : open file.err <build/errors_all.err>
[fileUtility.c] info : files are the same, keep the old one <src/sysCall/error.h>
[parseInitrc.c] info : open <src/hal/arch/avr8/arch_init.rc> for parsing
[parseInitrc.c] info : found 0 module
[parseInitrc.c] info : open <src/hal/mcu/atmega2560/mcu_init.rc> for parsing
[parseInitrc.c] info : found 4 module
[parseInitrc.c] info : open <src/hal/board/arduino_mega/board_init.rc> for parsing
[parseInitrc.c] info : found 6 module
[parseInitrc.c] info : open <src/services/services_init.rc> for parsing
[parseInitrc.c] info : found 2 module
[parseInitrc.c] info : open <src/tasks/tasks_init.rc> for parsing
[parseInitrc.c] info : found 2 module
[parseTag.c] info : open <src/sysCore/initSys.c> for parsing tag section
[parseTag.c] info : found tag threads init
[parseTag.c] info : end tag
[parseTag.c] info : found tag drivers init
[parseTag.c] info : end tag
[fileUtility.c] info : files are the same, keep the old one <src/sysCore/initSys.c>
[parseTag.c] info : open <src/sysCore/runLevel.c> for parsing tag section
[parseTag.c] info : found tag run levels
[parseTag.c] info : end tag
[fileUtility.c] info : files are the same, keep the old one <src/sysCore/runLevel.c>
[parseTag.c] info : open <src/sysCall/error.c> for parsing tag section
[parseTag.c] info : found tag error catalog
[parseTag.c] info : end tag
[fileUtility.c] info : files are the same, keep the old one <src/sysCall/error.c>
[parseTag.c] info : open <src/sysCore/TaskMate.c> for parsing tag section
[parseTag.c] info : found tag target name
[parseTag.c] info : end tag
[fileUtility.c] info : files are the same, keep the old one <src/sysCore/TaskMate.c>
[writeInclude.c] info : generate include statements in <src/sysCore/autoInclude_threads.h>
[fileUtility.c] info : files are the same, keep the old one <src/sysCore/autoInclude_threads.h>
[writeInclude.c] info : generate include statements in <src/hal/autoInclude_hal_user.h>
[fileUtility.c] info : files are the same, keep the old one <src/hal/autoInclude_hal_user.h>
[writeInclude.c] info : generate include statements in <src/hal/autoInclude_hal_system_critical.h>
[fileUtility.c] info : files are the same, keep the old one <src/hal/autoInclude_hal_system_critical.h>
[writeAlloc.c] info : generate allocation tables in <src/sysCore/autoAlloc.h>
[fileUtility.c] info : files are the same, keep the old one <src/sysCore/autoAlloc.h>
[fileUtility.c] info : *****************************************************
[fileUtility.c] info : summary of modified files : 0 updated, 9 unchanged
[fileUtility.c] info : *****************************************************
[printModules.c] info : found drivers :
	drivers[0] "hal_timerScheduler" status=1
	drivers[1] "hal_timerRTC" status=1
	drivers[2] "hal_i2c" status=1
	drivers[3] "hal_usart" status=1
	drivers[4] "hal_lcd" status=2
	drivers[5] "hal_ZS_042" status=2

[printModules.c] info : found services :
	services[0] "scli" status=3
	services[1] "msg" status=3

[printModules.c] info : found tasks :
	tasks[0] "task1" status=4
	tasks[1] "task2" status=4

[printModules.c] info : threads (services + tasks) by run level :
	run_level_threads_count[RUN_NONE] = 0
	run_level_threads_count[RUN_CORE] = 0
	run_level_threads_count[RUN_DRIVER] = 0
	run_level_threads_count[RUN_SERVICE] = 2
	run_level_threads_count[RUN_USER] = 4
```

This log demonstrates that:

- all *_init.rc files were found and parsed.
- each tag section was properly closed.
- all modules were listed with their respective status.
- thread counts per run level were consistent.

---

## 🧰 Reliability Features

- ✔️ Internal validation of tags, indexes and duplicate entries
- ✔️ Detailed logging with file and line number
- ✔️ Timestamped .autoCode_stamp file for Makefile dependency tracking
- ⚙️ Optional dry-run mode (--dry-run) for safe regeneration into .tmp files
- 🔒 Only replaces generated files when all steps complete successfully and if the new file differs from the old one
