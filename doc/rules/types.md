# TaskMate Type Rules

This guide defines type choices for embedded data and text across TaskMate layers.

## General types

- Use explicit fixed-width integer types from `<stdint.h>` for embedded data.
- Use `bool` from `<stdbool.h>` for boolean state.
- Name types in `snake_case` with a `_t` suffix.

## Text and bytes

- Use `char` only for text in the upper layers: tasks, services, `tmLibc`, and text-facing `sysCall` code.
- Use `uint8_t` for bytes, registers, wire data, buffers, and character codes in `sysCore` and HAL. Do not use `char`, `signed char`, or `unsigned char` as byte types there.
- Keep `interfaces/` contracts consumed by lower layers byte-oriented. They must not carry text as `char` data.
- Convert between text and bytes at the upper-layer boundary. Preserve the byte value and check bounds before storing a received byte in a text buffer.
- A HAL adapter for a host C library may cast to `char` only at the required library call. Keep the surrounding HAL state and transport APIs in `uint8_t`.
- The host-side `autoCode` program may use `char` for file paths, parsing, and generated text. Its emitted embedded contracts must follow the layer rules above.
