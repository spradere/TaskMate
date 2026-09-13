# 🔌 Interface Layer Rules

## Introduction

TaskMate separates **portable system contracts** from **hardware-specific implementations**.
To achieve this, the project defines a dedicated `interfaces/` directory containing **neutral interface
definitions** that are shared across all architectures, MCUs, and boards.

These interfaces describe **what the system expects**, while the Hardware Abstraction Layer (HAL)
implements **how the hardware fulfils those expectations**.

This separation keeps the architecture clear, prevents dependency inversions, and improves portability
when supporting new platforms.

Generic driver interface files use the `drv_<driver>.h` filename form. The following rules define how
the `interfaces/` layer must be used.

## Rule 1 — Interface independence

The `interfaces/` layer must not depend on any higher or lower system layer.

It may only include:

- basic C types
- minimal common headers

Examples: `stdint.h`, `stdbool.h`, `tm_types.h`

No dependency from `interfaces/` to HAL, sysCore, `sysCall`, `tmLibc`, services, or tasks is
allowed.

## Rule 2 — HAL dependency direction

The Hardware Abstraction Layer **may depend on** `interfaces/`, but must never depend on higher layers
 such as:

- `sysCall/`
- `services/`
- `tasks/`

The HAL implements the contracts defined in `interfaces/`, but it must remain completely unaware of
how higher layers use those interfaces.

## Rule 3 — System layer usage

HAL, sysCore, `sysCall`, `tmLibc`, and services may consume the neutral contracts permitted by
`conf/arch_valid_matrix.md`. Tasks use service, `sysCall`, or `tmLibc` APIs rather than including
`interfaces/` directly.

`interfaces/` remains the only transversal layer. `tmLibc` is a normal horizontal layer above
`sysCall`: it may consume interfaces and syscalls, but it must not depend on HAL or sysCore. The
dependency must never point from a lower layer back to `tmLibc`.

## Rule 4 — Hardware-specific interfaces

If an interface is specific to a particular architecture, MCU, or board, it **must not be** placed in
`interfaces/`.

Hardware-specific definitions belong in the **HAL layer**, where they remain isolated from portable
system contracts.

The `interfaces/` directory must only contain definitions that are meaningful across all supported
platforms.
