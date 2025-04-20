# TaskMate Makefile Summary

## ✔️ Features

- **Automatic Dependency Handling**: Only recompiles modified source files.
- **Header Dependency Tracking**: Detects changes in `.h` files and recompiles affected `.c` files.
- **Automated Code Generation**:
  - If any *_init.rc have been updated run autoCode before compilation.
- **Colorized Output**: Improved readability with `@printf` messages.
- **POSIX-Compliant**: Works without requiring GNU Make.

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
| `make doc` | Generate Doxygen documentation |
| `make dump` | Disassemble machine code in two formats |
| `make cloc` | Count lines of code |
| `make tidy` | C code static analysis with clang-tidy tool, config file .clang-tidy |
| `make format` | C code formating with clang-format tool, config file .clang-format|

## ✈ Streamlined Workflow

Short, intuitive commands for development, deployment, and backup.


