# TaskMate Makefile Summary

## Features

- **Automatic Dependency Handling**: Only recompiles modified source files.
- **Header Dependency Tracking**: Detects changes in `.h` files and recompiles affected `.c` files.
- **Multi-Compiler Setup**:
  - Uses `clang` for `TaskMate_tag_expand.c`
  - Uses `avr-gcc` for AVR sources.
- **Automated Code Generation**:
  - Expands task/driver lists from `TaskMate_tag_expand.c` before compilation.
- **Colorized Output**: Improved readability with `@printf` messages.
- **POSIX-Compliant**: Works on FreeBSD without requiring GNU Make.

## Usage

| Command | Description |
|---------|-------------|
| `make` | Compiles the project. |
| `make upload` | Uploads the compiled firmware to the device. |
| `make push M="message"` | Commits and pushes changes to Git with a message. |
| `make backup` | Backs up the project to a USB key. |
| `make doc` | Generate Doxygen documentation. | 

## 🔧 How It Works

1. **Modifies Only Necessary Files**
   - Detects changes in headers and recompiles only relevant `.c` files.
2. **Handles Auto-Generated Code**
   - Runs `TaskMate_tag_expand.c` first (with `clang`), then compiles everything else.
3. **Streamlined Workflow**
   - Short, intuitive commands for development, deployment, and backup.


