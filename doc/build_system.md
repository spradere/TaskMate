# 🏗️ TaskMate build system


## ✨ Makefile features

- **Automatic Dependency Handling**: Only recompile modified source files.
- **Header Dependency Tracking**: Detects changes in `.c .h` files and recompile affected `.c` files.
- **Automated Code Generation**:
  - If any *_init.rc have been updated autoCode is launched before compilation.
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

Miscellaneous commands :

| Command | Description |
|---------|-------------|
| `make clean` | Delete files, except code, obviously ! |
| `make doc` | Generate Doxygen documentation |
| `make dump` | Disassemble machine code |
| `make cloc` | Count lines of code |
| `make tidy` | C code static analysis with clang-tidy tool, config file /.clang-tidy |
| `make note` | find todo / fix / hack in sources files |
| `make check` | C code static analysis with cppcheck tool |
| `make format` | C code formatting with clang-format tool, config file /.clang-format|

---

## 🚀 Streamlined Workflow

Short, intuitive commands for development, deployment, and backup.

---

## 🛠️ Complete build system
```
              +-----------------------+
              |        make           |
              |  Build Orchestration  |
              +----------+------------+
                         |
                         | selects architecture, MCU, board
                         | and all relevant source files
                         | manage dependencies .c <-> .h
                         | provides utilities
                         |
                         v
              +-----------------------+
              |      autoCode         |
              |Dynamic Code Generation|
              +-----------------------+
                         |
                         | parses all *_init.rc files
                         | generates headers autoInclude_*.h
                         | generate code in .c files via tag system
                         |
                         | autoCode check configuration validity:
                         |   - files name
                         |   - options
                         |   - modules count / name
                         v
              +-----------------------+
              |       Compiler        |
              +-----------------------+
                         |
                         | each translation unit
                         | becomes an object file:
                         |   foo.c → foo.o
                         |
                         | compiler verifies:
                         |   - syntax & types
                         |   - correct headers
                         |   - matching prototypes
                         |
                         v
              +-----------------------+
              |        Linker         |
              +-----------------------+
                         |
                         | merges all .o files
                         | resolves symbols:
                         |   - strong vs weak definitions
                         |   - missing symbols
                         |
                         | detects errors such as:
                         |   undefined reference
                         |   multiple definition
                         |
                         v
              +-----------------------+
              |     Final Firmware    |
              +-----------------------+
```

---

## 🚧 Architecture Boundary Enforcement in the TaskMate Build System

TaskMate implements a lightweight but highly effective *compile-time architecture
boundary system* designed to prevent accidental or unauthorised access to
system-critical HAL and sysCore interfaces. This mechanism relies entirely on
static checks and conventions, adding **zero overhead** to the final binary and no **runtime cost**.

The enforcement logic operates at **two complementary levels:**

---

### 1. Build-Time Include Validation

All system-critical headers are declared in a single configuration file `allow.mk`
as pattern/whitelist pairs. During the build, a dedicated Make rule scans the source
tree with `grep` and ensures that each critical header is included **only** by source
files explicitly listed in the whitelist.

This mechanism automatically extracts the set of `<pattern → allowed>` pairs from
the `.mk` file, avoiding duplication and ensuring that the configuration itself
remains the single source of truth. Any unauthorised include immediately stops
the build, guaranteeing a clean architectural boundary before compilation ever begins.

---

### 2. Compile-Time Protection in Critical Headers

Each system-critical header contains a defensive guard such as:

```C
#ifndef HAL_SYSTEM_CRITICAL_ALLOWED
	#error "Forbidden inclusion of system-critical headers"
#endif
```

Only the modules explicitly authorised receive the appropriate `-DX_Y_ALLOWED`
flag from the build system. This ensures that **both direct and recursive includes**
are kept under strict control, even if a developer manually bypasses the build-time whitelist.

---

### 3. Structural and Naming Conventions

TaskMate reinforces its internal boundaries through clear file naming
`(hal_usr_api.h, hal_system_critical_api.h, autoInclude_*.h)` and a consistent
directory hierarchy `(sysCore/, sysCall/, hal/, services/, tasks/)`. The architecture itself
makes unauthorised use of privileged interfaces unlikely, and the automated checks
eliminate the remaining risk.

---

### 4. Software-Only Boundary — Not a Hardware MMU

This mechanism enforces **logical** and **code-structure** separation between system-level
and user-level components. However, it remains **purely software-based:**

- It does **not** provide memory isolation or privilege rings.
- It is **not equivalent** to a hardware MMU, MPU, or segmentation unit.
- It does **not** prevent malicious code from misbehaving if intentionally modified.

Its purpose is to enforce *architectural discipline*, not to provide hardware-backed
process isolation. In the context of embedded systems—where the developer
controls the tool-chain and firmware image—this approach provides an excellent
balance between **robustness**, **clarity**, **maintainability**, and zero **runtime cost**.
