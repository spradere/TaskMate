# TaskMate Makefile Summary

## ✔️ Features

- **Automatic Dependency Handling**: Only recompiles modified source files.
- **Header Dependency Tracking**: Detects changes in `.h` files and recompiles affected `.c` files.
- **Multi-Compiler Setup**:
  - Uses `clang` for `TaskMate_tag_expand.c`
  - Uses `avr-gcc` for AVR sources.
- **Automated Code Generation**:
  - Expands task/driver lists from `TaskMate_tag_expand.c` before compilation.
- **Colorized Output**: Improved readability with `@printf` messages.
- **POSIX-Compliant**: Works on FreeBSD without requiring GNU Make.

## ✍️ Usage

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
| `make doc` | Generate Doxygen documentation. | 
| `make dump` | Disassemble machine code in two formats, for debugging purpose |


## ✈ Streamlined Workflow

Short, intuitive commands for development, deployment, and backup.


