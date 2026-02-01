# 🏗️ TaskMate build system


## ✨ Makefile features

- **Automatic Dependency Handling**: Only recompile modified source files.
Detects changes in `.c .h` files and recompile affected `.c` files.
- **Automated Code Generation**:
If any init.rc or error.err files have been updated autoCode is launched before compilation.
- **Colourised Output**: Improved readability with `@printf` messages.
- **POSIX-Compliant**: Works without requiring GNU Make.

---

## ✍️ Makefile usage

Main workflow :

| Command | Description |
|---------|-------------|
| `make` | Compiles the project. |
| `make upload` | Uploads the compiled firmware to the device. |
| `make push M="message"` | Commits and pushes changes to Git with a message. |
| `make backup` | Backs up the project to a USB key. |
| `make help` | print targets help. |

TaskMate exposes a concise set of user `make` targets.
The authoritative and up-to-date list of available commands
is provided by `make help`, generated dynamically from the Makefile itself.

🚀 **Streamlined workflow** : short, intuitive commands for development, deployment, and backup.

---

## 🛠️ Complete build system

![Build system](doc/build_v2.png)

---

## 🚧 Architecture boundary enforcement

TaskMate enforces architectural boundaries **at build time** to prevent accidental
or unauthorised access to system-critical interfaces.
This mechanism relies entirely on **static checks and conventions**,
introducing **zero runtime overhead** and no impact on the final firmware size.

Boundary enforcement is handled directly by the build system and operates at three complementary levels:


### 1. Build-time validation


The Makefile validates which source files are allowed to include system-critical
headers, based on explicit white list rules. Any unauthorised inclusion immediately
stops the build, ensuring architectural violations are detected **before compilation.**


### 2. Compile-time guards (temporarily out of use due to headers refactor)

System-critical headers contain defensive compile-time checks. Only explicitly
authorised modules receive the required preprocessor definitions, preventing both
 direct and indirect inclusion of privileged interfaces.


### 3. Structural conventions

Clear directory separation and consistent naming conventions reinforce architectural
 discipline, making incorrect usage unlikely and easy to detect.


### Software-only boundary

This mechanism enforces **logical and structural separation** only. It does **not** provide
 memory isolation, privilege rings, or hardware-backed protection (MMU/MPU).
 Its purpose is to maintain architectural clarity, robustness, and long-term
 maintainability within the constraints of embedded systems, while remaining
 lightweight and fully deterministic.
