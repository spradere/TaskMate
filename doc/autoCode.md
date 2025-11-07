# 👨‍💻 autoCode: configuration-driven code generator

## 👀 overview

> 🚨 ⚠️ <span style="color:red"> autoCode is a critical component: if it generates incorrect code,
> TaskMate may still compile but will behave unpredictably at runtime.
> Any change to autoCode must be considered system-critical and tested accordingly.</span>

`autoCode` is a **code generation tool** used by TaskMate to automatically build
the initialization code for drivers, services, and user tasks.
It consolidates all module definitions from `*_init.rc` configuration files
into the generated system files that TaskMate uses at runtime.

---

## ❓ How it's work ?

`autoCode` is responsible for generating dense, low-level initialization
code in files like `initSys.c` and a set of auto-generated headers (`autoInclude.h` & `autoAlloc.h`).

This generated code is **not meant to be human-friendly**: it uses indexes,
tables, and “magic numbers” to keep the runtime overhead small and the layout explicit for the compiler.

Instead of relying on the readability of the generated code, TaskMate trusts the **generator** itself:

- autoCode parses architecture-specific and generic *_init.rc files (drivers, services, tasks).
- It validates tags, run levels and module definitions.
- It prints a detailed, human-readable summary of all discovered modules and their run-level mapping.


In other words, autoCode acts both as a **configuration validator** and as
a **single source of truth** for system initialization. As long as autoCode
is correct and the .rc files are consistent, the generated code can remain
dense and optimized without impacting maintainability.

---

## 📁 Input Files

`autoCode` reads `*_init.rc` initialization files describing which modules are active
for the target architecture and how they should be initialized.

| Type        | Typical Path                          | Description                              |
|--------------|----------------------------------------|------------------------------------------|
| Drivers      | `src/arch/<arch_name>/drivers_init.rc` | Hardware-dependent drivers (I2C, UART…)  |
| Services     | `src/services/services_init.rc`        | System-level services (CLI, msg server)  |
| User Tasks   | `src/tasks/tasks_init.rc`              | User-level or application tasks          |

The target architecture is usually passed from the Makefile,
and determines which folder under `src/arch/` is parsed.

---

## 📤 Output Files

`autoCode` generates several key files that are later compiled into TaskMate:

| File                        | Role                                                                 |
|------------------------------|----------------------------------------------------------------------|
| `include/autoInclude.h`      | Centralized header including all driver/service/task headers.        |
| `include/autoAlloc.h`        | Static allocation tables for modules.                                |
| `src/sysCore/initSys.c`      | Low-level initialization routines executed during system startup.    |

---

## 📝 Logging Example

A typical autoCode run looks like this:

```
[autoCode.c:61] info : arch_name
         <avr8>

[parseInitrc.c:30] info : open init.rc file for parsing
         <src/arch/avr8/drivers_init.rc>

[parseInitrc.c:30] info : open init.rc file for parsing
         <src/services/services_init.rc>

[parseInitrc.c:30] info : open init.rc file for parsing
         <src/tasks/tasks_init.rc>

[parseTag.c:80] info : found tag :
         threads init
[parseTag.c:104] info : end tag

[parseTag.c:80] info : found tag :
         drivers init
[parseTag.c:104] info : end tag

[parseTag.c:80] info : found tag :
         run levels
[parseTag.c:104] info : end tag

[printModules.c:27] info : found drivers :
        drivers[0] "timer1" status=1
        drivers[1] "timer3" status=1
        drivers[2] "i2c" status=1
        drivers[3] "usart1" status=1
        drivers[4] "lcdAMC2004" status=2

[printModules.c:35] info : found services :
        services[0] "scli" status=3
        services[1] "msg" status=3

[printModules.c:44] info : found tasks :
        tasks[0] "task1" status=4
        tasks[1] "task2" status=4

[printModules.c:52] info : threads (services + tasks) by run level :
        run_level_threads_count[RUN_NONE] = 0
        run_level_threads_count[RUN_CORE] = 0
        run_level_threads_count[RUN_DRIVER] = 0
        run_level_threads_count[RUN_SERVICE] = 2
        run_level_threads_count[RUN_USER] = 2

touch .autoCode_stamp
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
- 🔒 Only replaces generated files when all steps complete successfully
