# General build file and directory safety audit

Date: 13 September 2026

Audited branch: `codex/build-audit`

Audited revision: `38a2280a8dd5ab7cb4f32d5fa6b814b7ca240174`

Environment: FreeBSD 16.0-CURRENT, BSD `bmake` 20260704

## Purpose and position on portability

This audit primarily examines file creation, replacement, removal, and copying, path construction, temporary files, interruptions, and concurrent execution. It covers the main Makefile, `mk/*.mk`, selected hardware fragments, shell and AWK scripts, and the autoCode file subsystem.

The deliberate use of BSD `bmake` and FreeBSD is considered a valid design constraint. Adding GNU Make, Linux, CMake, or another orchestrator is not recommended at this stage. Limiting the platform reduces the validation matrix and focuses effort on system code. The extensions used, including `.WAIT`, `:T`, `:ts`, `find -delete`, and `realpath`, should simply be accepted, documented, and tested on that platform.

## Threat model

The build runs with the developer's permissions, and its Makefiles are executable code. It cannot provide a security boundary against an intentionally malicious Git branch. The realistic goal is to protect developer files against:

- an empty, incorrect, or overridden variable;
- an unexpected path or symbolic link;
- interruption, a full disk, or an I/O error;
- two builds running concurrently;
- a malformed configuration file;
- a backup directed to the wrong file system.

The safety findings below assume an ordinarily trusted repository, but not a perfectly reliable environment.

## Verdict

The foundation is sound for local, single-user, sequential use. Removals are now preceded by canonical confinement under `build/`, autoCode writes are deferred, read errors are bounded, and ordinary artifacts are separated from sources.

The build is not yet robust against its most important file errors. The main risk is the `backup` target, which uses `rsync --delete` without proving that the mount point exactly matches the expected device. Other risks are an architecture check whose failure status is masked, predictable temporary names, several non-atomic writes, and no contract against concurrent builds.

| No. | Priority | Finding | Main consequence |
|---|---|---|---|
| 1 | Critical | `backup` does not validate the exact mount before `rsync --delete` | Possible deletion in a local disk directory or on the wrong volume |
| 3 | High | `.tmp` files are predictable and sometimes published without immediately stopping on error | Overwrite, race, symbolic link, or partial publication |
| 4 | High | autoCode lists and `.gitignore` are written directly | An interruption can leave a partial file considered up to date |
| 5 | Medium | autoCode replaces several destinations without a global transaction | A late error leaves a mixed generated set |
| 6 | Medium | The graph does not prohibit parallel or simultaneous builds | Races on generated sources, logs, stamps, and results |
| 10 | Low | Some checks construct a shell command from configuration | Reduced robustness with special characters and indirect diagnostics |

## Strengths

### Removal confinement

`scripts/check_build_delete_path.sh` resolves its own location, derives the canonical `build/` root, rejects empty values, and rejects every external target. The `build/` root itself is accepted only with `--allow-build-root`. The script uses `set -eu`, and calls from `clean`, `clean_hard`, and `autoCode_alone` quote their paths (`scripts/check_build_delete_path.sh:13-68`, `mk/utils.mk:15-79`, `mk/autoCode.mk:153-162`).

Destructive commands also use useful limits such as `-type f`, `-maxdepth 1`, or `-mindepth 1`, depending on the contract. The previous audit's critical `find -delete` risk, where a direct override could point to `/` or an external path, is fixed in the current revision.

During focused validation, the guard accepted expected descendants and rejected `build/` without the explicit option, as well as `/tmp`. It greatly reduces variable-related accidents. It is not an atomic primitive. Another process can still change a path component between validation and `find`.

### autoCode processing before publication

autoCode reads files line by line with a bounded size and distinguishes end of file, truncation, and error (`srcs/autoCode/fileUtility.c:167-206`). It first generates a temporary file, closes streams, verifies required tags, then starts comparison and replacement only after analysis (`srcs/autoCode/autoCode.c:135-165`, `srcs/autoCode/parseTag.c:133-258`).

Replacement of a modified destination directly uses `rename(temporary, destination)`. A rename error therefore does not first remove the original. Registered temporary files are cleaned after processing and through `atexit()` (`srcs/autoCode/fileUtility.c:65-164,305-385`). Stream and `fclose()` errors are detected. The test corpus checks that a parse error does not modify an earlier destination and that no `.tmp` remains (`test/autoCode/autoCode_test.sh:91-117,433-475`).

### Explicit scope and dependencies

Main paths are centralised in `mk/path_files.mk`. The target, board, MCU, and architecture fragments are included explicitly, and missing target values are rejected while parsing the Makefile. A script checks required programs by reading each line literally and quoting its argument (`scripts/check_programs.sh:17-44`). The build downloads neither dependencies nor code during execution.

## Detailed findings

### 1. Critical: backup can delete on the wrong file system

The `backup` target decides that the device is mounted with:

```sh
mount | grep -q "${PATH_USBKEY}"
```

This searches for a substring in all `mount` output. It verifies neither exact mount-point equality, the `${FILE_USBDEV}` device, nor the `msdosfs` type. Another mount whose line contains `/media/usbkey` can therefore create a false positive. The recipe then creates the directory and runs `rsync --delete --delete-excluded` (`mk/backup.mk:39-70`). If the device is not actually mounted, `/media/usbkey/...` may belong to the local file system, and its content may be deleted to mirror the repository.

The target also always unmounts `${PATH_USBKEY}`, even if it was mounted before invocation. Conversely, an `rsync` failure stops the recipe before `umount` because there is no exit handler. Finally, the name "backup" hides mirror semantics. Repeating the target for the same version deletes mirror files that were removed from the source. It is not an immutable snapshot.

Recommended minimum fix:

1. read the FreeBSD mount table in a parseable format and require exact equality for the device, mount point, and type;
2. record whether the recipe mounted the volume itself;
3. install a `trap` that unmounts only in that case;
4. canonicalise the destination and prove that it is a strict descendant of the mount point;
5. require a device-specific sentinel, then first run an operator-visible `rsync --dry-run`;
6. explicitly document the "destructive mirror" choice, or remove `--delete` if history is expected.

This fix can remain entirely FreeBSD and POSIX based and does not justify changing the build system.

### 3. High: predictable temporary names and non-exclusive opening

Three mechanisms use a fixed `.tmp` suffix:

- autoCode creates `<destination>.tmp` and opens it with `fopen(..., "w+")` (`srcs/autoCode/fileUtility.c:289-303,375-385`);
- `scripts/compare_replace.sh` writes `${destination}.tmp` (`scripts/compare_replace.sh:17-23`);
- `.BEGIN` writes `srcs/interfaces/tm_info.h.tmp` (`mk/build.mk:20-39`).

These opens truncate an existing file and follow symbolic links. Two concurrent builds write the same temporary file. A stop can leave a `.tmp` inherited by the next invocation. autoCode registers and cleans its temporary files more carefully, but does not use exclusive creation or a unique name. Replacement also changes the destination mode according to the `umask` that created the temporary file.

`compare_replace.sh` is more fragile. It has no argument validation, `set -e`, or `trap`. If its `printf` fails, for example because the disk is full, the script continues to `cmp` and may rename a partial or old temporary file over the valid file.

Temporary files should be created in the destination directory with `mktemp` in shell and `mkstemp(3)` in C. Their names should be retained in a `trap` or registry, every close checked, and publication performed with `rename`. autoCode should check destination type with `lstat` and `fstat`, define an explicit symbolic-link policy, and preserve the expected mode.

### 4. High: several control files are truncated in place

The `files_error`, `files_initrc`, `files_to_parse`, `files_halinit`, `values_funcinit`, and `files_haldefine` lists are emptied and then filled line by line at their final locations (`mk/autoCode.mk:116-150`). An interruption leaves a partial list with a recent timestamp. On the next run, `bmake` may consider it current, and autoCode may process an incomplete composition.

`.gitignore` generation follows the same pattern (`mk/backup.mk:15-37`). An interruption may temporarily change the visibility of many artifacts and make an overly broad Git command more risky. `build/last_build_info.txt`, CLOC data, and several logs are also written in place. Their impact is lower, but diagnostics may become inconsistent.

Each generator should write an adjacent temporary file, verify complete success, then use `mv` or `rename`. Lists should also be sorted when order has no semantic meaning. Because `*.rc` order affects composition, it must either be explicitly defined or documented as a contract, not left to `find` enumeration.

### 5. Medium: autoCode publication is atomic only per file

All inputs are analysed before publication, which protects well against syntax errors. However, `fileCmpReplaceAll()` replaces destinations one by one. If the third `rename` fails, the first two destinations remain new and later ones remain old (`srcs/autoCode/fileUtility.c:65-101`). The build fails, but the source tree is partially updated. The architecture note already recognises this lack of rollback.

Two strategies are acceptable:

- explicitly accept "atomic per file, blocked build for a mixed set", then guarantee that the next autoCode run always repairs the set;
- add a small transaction journal and adjacent backups that support rollback.

The first option is probably proportionate for the current embedded project, provided a test injects a `rename` failure after at least one success.

### 6. Medium: parallel builds have no safe contract

The graph uses `.WAIT`, but declares no `.NOTPARALLEL`, per-target lock, or session directory. With `bmake -j`, `_system_critical_check` and `_autocode` belong to the same group before the first `.WAIT` (`mk/build.mk:81-83`). The check may read sources while autoCode replaces them. After the second `.WAIT`, linking, memory measurement, and CLOC are also sibling prerequisites. `_mcu_memory_data` does not explicitly depend on the linked binary (`srcs/hal/arch/avr8/avr8_CC.mk:59-66`).

Two simultaneous invocations on the same target also share `.tmp` files, stamps, logs, and generated source files. Different targets still share `build/autoCode`, `build/log`, and the same generated destinations.

In the short term, declaring the build non-parallel and rejecting two instances with a FreeBSD lock is simpler than making the whole chain reentrant. `.WAIT` should continue to express real dependencies, especially linked binary before memory measurement. Selective parallel compilation can be reintroduced later if it provides a measured gain.

### 10. Low: the header check constructs a shell command

`header_allow.awk` concatenates `source_file` and `PATH_SOURCES` into a string passed to a `grep` shell pipe (`scripts/header_allow.awk:143-168`). The format permits more characters than this construction can quote. In a trusted repository, injection is not a real security boundary, but an accidental quote or metacharacter can change the scan.

The check also searches for a string in all content, not a lexical `#include` directive, so it may include comments and strings. It would be more robust for AWK to traverse the source list directly, like the architecture checker, without constructing a shell command.

## Secondary observations

- The program-check stamp avoids repeated cost, but does not detect a change in `PATH` or a tool version. This is not a deletion risk. The stamp only needs to be removed or invalidated when diagnosing the environment.
- Historical absolute paths from `cppcheck` and `tidy_autoCode` do not match the current checkout. This harms reproducibility without directly creating a destructive risk.
- The build injects the Git version, counter, and date into its outputs. This provenance is useful, but the result is intentionally not bit-for-bit reproducible.

## Recommended hardening plan

### Step 0: short and urgent fixes

1. Propagate the architecture check's non-zero status to `bmake`.
2. Fix the log-removal pattern.
3. Harden `backup` before its next use: exact mount, sentinel, canonical destination, unmount handling, and an explicit decision about `--delete`.
4. Add `set -eu`, argument validation, and a unique temporary file to `compare_replace.sh`.

### Step 1: safe file publication

1. Introduce one shell helper for "write, close, compare, rename" with an adjacent temporary file.
2. Use it for autoCode lists, `.gitignore`, `tm_info.h`, and metadata files.
3. Replace `<destination>.tmp` with `mkstemp(3)` in autoCode, check types, and preserve mode.
4. Define and test the symbolic-link policy.

### Step 2: determinism and concurrency

1. Officially declare the complete build non-parallel and add a per-checkout lock.
2. Fix graph dependencies before re-enabling `-j`.
3. Fix discovery order or explicitly declare lists whose order is semantic.
4. Add a lightweight validator for supported path names.

### Step 3: failure-path validation

Add isolated tests under a temporary directory for a simulated full disk, open denial, close failure, pre-existing temporary file, symbolic link, interruption, two concurrent processes, failure of the second `rename`, path with spaces, absent mount, and incorrect backup destination. Backup tests must use a test tree or file system and must never mount or delete the operator's real data.

## Validation performed during the audit

- read the `build` and `autoCode` architecture documents, style and interface rules, and applicable Makefiles and scripts;
- inspected values expanded by `bmake -V` for the default target;
- expanded `bmake -n help` without execution, confirming the global effects of `.BEGIN` and `.END`;
- directly ran the architecture checker read-only: status `3`, 13 violations;
- ran `sh -n` syntax checks for every shell script in scope;
- focused validation of the guard: descendants of `build/` accepted, `build/` and `/tmp` rejected;
- verified the current absence of symbolic links under `build/` and residual `.tmp` files in the repository outside `.git`.

No `clean`, `clean_hard`, `backup`, autoCode, AVR build, or generator test was run. Those commands would have modified artifacts or generated sources and were unnecessary to establish the findings. No physical hardware validation applies to this audit.

## Conclusion

Remaining on `bmake` and FreeBSD is reasonable and even beneficial in the short term. The priority is not build portability. It is reducing the number of write paths and turning each into a confined, checked, atomically published operation.

The project has already addressed the most dangerous `clean` issue with the canonical guard. The next safety improvement comes from focused `backup` hardening, strict error-status propagation, unique temporary files, and an explicit sequential contract. These changes remain small and local to the host build, with no portability cost for embedded RTOS code.
