# 🏗️ Architecture Note — build

## Historical developments
TaskMate’s build system has evolved from a single entry Makefile into a layered BSD `bmake` stack split by concern: global orchestration (`Makefile`), reusable infrastructure (`mk/*.mk`), and target-dependent extension points (`src/hal/**/**_make.mk`). The current structure reflects two parallel needs: (1) deterministic firmware build for embedded targets and (2) host-side project tooling (autoCode generation, static checks, docs, editor helpers, and backup workflows).

A key step in this evolution is the integration of `autoCode` as a first-class build phase and the explicit `ARCH -> MCU -> BOARD` target validation path. Together, these choices move configuration errors and integration drift from runtime toward build time.

After v0.28, the source tree was separated into system, user, HAL, interfaces, and `tm_libc`
domains. Target selection moved to `user/target/<HWT>/hwt.mk`, which includes the matching board, MCU,
and architecture fragments. Header allow-list parsing moved to a dedicated AWK script, compiler warning
coverage was expanded, and Make variable names were normalised by role (`PATH_`, `FILES_`, `VAL_`,
`OPT_`, and `CFLAGS_`).

## Current implementation
The default `HWT=test1` selects `srcs/user/target/test1/hwt.mk`. That file adds target sources and
includes `board_make.mk`; the board fragment selects the ATmega2560 MCU, and the MCU fragment selects
the AVR8 architecture and compiler configuration. These fragments accumulate source roots, generated
HAL include lists, preprocessor target symbols, memory limits, programmer settings, and AVR compiler
flags.

The `all` target runs the generated `.gitignore` rule, the architecture-boundary check, target-scoped
autoCode generation, dependency aggregation, AVR compilation/linking, memory-usage extraction, and
line counting. Objects, dependency files, maps, firmware output, autoCode configuration, and stamps live
under `build/`, with firmware artifacts separated by the selected hardware stack. Compile-time guards
protect critical headers, while `scripts/header_allow.awk` scans the source tree against
`mk/header_allow.conf` before compilation.

## Well-built code and implementation weaknesses
### Strengths
- Build, source discovery, hardware selection, policy checks, utilities, and backup rules are separated
  into focused Make fragments.
- A missing hardware target, missing GPIO declaration file, unavailable HAL implementation, or forbidden
  critical include fails before firmware execution.
- Target-specific objects and reports stay under a predictable `build/<hardware-stack>/` path.
- The AVR configuration enables a broad warning set, link-time optimisation, section garbage collection,
  dependency files, and explicit flash/RAM reporting.

### Remaining weaknesses
- The build depends on BSD `bmake`, many Unix utilities, Git state, AVR tools, and host-installed analysis
  tools; several analysis, backup, USB, and programmer paths are still machine-specific.
- The header allow-list check searches text with `grep`. It can match comments, cannot resolve indirect
  preprocessor dependencies, and protects only patterns explicitly listed in its configuration.
- Target compatibility is encoded by nested Make includes rather than by a separately validated
  capability model; only the `test1 / arduinoMega / atmega2560 / avr8` stack is present today.
- The autoCode executable rule refers to misspelled prerequisite variables, so changes to generator
  sources are not reliably represented by that rule's dependency list.
- Build metadata includes dates, Git descriptions, and an incrementing revision count; reproducible
  firmware has not yet been demonstrated with pinned tool versions and clean generated-state checks.
