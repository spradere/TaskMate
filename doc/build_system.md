# 🏗️ TaskMate build system


## ✨ Makefile features

- **Automatic Dependency Handling**: Only recompiles modified source files.
- **Header Dependency Tracking**: Detects changes in `.h` files and recompiles affected `.c` files.
- **Automated Code Generation**:
  - If any *_init.rc have been updated run autoCode before compilation.
- **Colorized Output**: Improved readability with `@printf` messages.
- **POSIX-Compliant**: Works without requiring GNU Make.

---

## ✍️ Makefile Usage

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
| `make format` | C code formating with clang-format tool, config file /.clang-format|

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
                         | parses drivers_init.rc and tasks_init.rc
                         | generates autoInclude_system.h, autoInclude_hal.h and autoAlloc.h
                         | generate code in initSys.c and runLevel.c
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
                         |   - dead code removal (LTO)
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
