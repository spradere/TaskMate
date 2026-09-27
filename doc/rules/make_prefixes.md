# 🏷️ Makefile variable prefix guide

This project uses prefixed variable names to make Makefiles easier to read and safer to extend.

## Practical naming rule

When adding new variables:

- Use `PATH_` for one directory, `PATHS_` for a list of directories.
- Use `FILE_` for one file, `FILES_` for a list/glob-derived set.
- Use `CONF_` for a configuration file consumed by the build.
- Use `SCRIPT_` for an executable script invoked or loaded by Make.
- Use `VAL_` for non-path scalar values.
- Use `OPT_` for toggles/options expected to be user-tunable.

## How to use them

1. **Define paths first**, then derive files from those paths.
   - `PATH_*` variables are declared in `Makefile` and `mk/path_files.mk` and reused everywhere.
2. **Use `CONF_*` for declarative build configuration files.**
   - Keep generated artifacts and other individual files under `FILE_*`; reserve `CONF_*`
     for files that define build policy, supported targets, or required tooling.
3. **Treat `VAL_*` as source of truth** for dynamic build metadata and hardware settings.
   - Example: `VAL_TM_VERSION` and `VAL_BUILD_CNT` are injected into compiler flags.
4. **Put optional behaviour behind `OPT_*`**, and validate accepted values.
   - This pattern is used in `mk/options.mk`.
5. **Use `FILES_*` for generated lists** (sources, headers, deps) and for target prerequisites.
   - This keeps rules concise and avoids path duplication.
6. **Use `SCRIPT_*` whenever Make invokes or loads a script.**
   - Keep every script path in `mk/path_files.mk`,
     then reference the variable in recipes and prerequisites.
7. **Use per-file `CFLAGS_*` only for exceptions** (e.g., specific privileged modules).
   - Keep global flags in common `CFLAGS` and local deviations in `CFLAGS_<source>`.


This keeps variable intent obvious during maintenance and hardware-target composition.
