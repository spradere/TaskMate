# TaskMate statistics history

Versions `v0.1` to `v0.4` come from `legacy/`. Versions `v0.5` to `v0.10` are Git tags.
As no `v0.10_test` ref exists locally or on `origin`, that column uses `ba2975e`, the direct
parent of `v0.11` and therefore the last test state before that release.

Values through `v0.10_test` were measured from isolated source copies with the current
instrumentation and AVR-GCC 14.2.0. Source files and lines use the current CLOC filters and
`scripts/cloc_data.awk`; Flash is `.text + .data`, and static RAM is `.data + .bss`. The old
Makefiles required their expected build directories and include roots to be supplied. Version
`v0.4` additionally required AVR-G++ in C++11 mode because `uart0.4.c` contains C++ member
initializers. Later values remain copied from the `README.md` history and were not recalculated.
An em dash means that a value is not available.

| Version | v0.1 | v0.2 | v0.3 | v0.4 | v0.5 | v0.6 | v0.7 | v0.8 | v0.9 | v0.10 | v0.10_test | v0.11 | v0.20 | v0.21 | v0.22 | v0.23 | v0.24 | v0.25 | v0.26 | v0.27 | v0.28 | v0.29 | v0.30 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Commits count | — | — | — | — | 1 | 9 | 22 | 35 | 44 | 50 | 90 | 150 | 167 | 192 | 192 | 22 | 245 | 280 | 329 | 442 | 498 | 652 | 983 |
| Source files count | 7 | 8 | 12 | 15 | 12 | 17 | 19 | 20 | 26 | 28 | 54 | 61 | 84 | 91 | 91 | 100 | 109 | 92 | 104 | 119 | 131 | 145 | 141 |
| Lines of code count | 278 | 278 | 341 | 421 | 339 | 431 | 951 | 1370 | 1784 | 1913 | 2948 | 4402 | 2593 | 2589 | 2589 | 3303 | 3491 | 2830 | 3461 | 6762 | 7302 | 8364 | 12501 |
| Binary size (Flash, bytes) | 870 | 866 | 1104 | 1284 | 1104 | 1204 | 1504 | 1736 | 2738 | 2906 | 2924 | 3286 | 3286 | 3352 | 3352 | 6070 | 6070 | 6022 | 6496 | 6496 | 6432 | 11080 | 15588 |
| RAM usage (bytes) | 519 | 524 | 524 | 1045 | 524 | 526 | 568 | 586 | 1692 | 1704 | 1758 | 1903 | 1903 | 1911 | 1911 | 2306 | 2306 | 2326 | 2009 | 2009 | 2013 | 1866 | 1820 |
