# Windows 10, MSYS2, and UCRT64 build port audit

Date: 3 October 2026

Re-evaluated branch: `test`

Re-evaluated revision: `496660f3d2ccf218ce64423955d8b6c575c278ee`

Original audited revision: `d0fabace2babc6fb61828d30b737ea0125c6639e`

Observed host: Windows 10 Pro 22H2, build 19045.6466, Cygwin 3.6.10,
`bmake` 20240314

## Purpose and scope

This audit re-evaluates the feasibility and procedure for running the TaskMate build under
MSYS2 UCRT64 on Windows 10. It covers BSD `bmake`, autoCode, shell and AWK scripts, AVR
compilation, host tests, quality tools, Arduino Mega flashing, and supporting utilities.

The embedded target remains `test1 -> avr8 / atmega2560 / Arduino Mega`. No Windows API is
required in the firmware. The port concerns the build host and must preserve the same selected
sources, generated contracts, compiler options, boundary checks, and deterministic behaviour.

The re-evaluation inspected the current Makefiles, scripts, tests, architecture notes, official
MSYS2 package data, and the cited pkgsrc bootstrap report. It also ran the available Cygwin host
tests. MSYS2 is not installed on the observed host. No UCRT64 build, sanitizer run, upload, or
hardware test was performed.

## Verdict

**The MSYS2/UCRT64 port remains feasible, but the repository is not ready for a qualifying
pilot. The current technical risk is high until a supported `bmake` artifact and a clean
host-test baseline exist.**

The recommended design remains a hybrid environment:

- MSYS supplies `sh`, AWK, `find`, `grep`, `sed`, and other POSIX utilities;
- UCRT64 supplies native Clang, the AVR toolchain, Cppcheck, and `avrdude`;
- TaskMate supplies a pinned and checksummed MSYS-compatible `bmake` package;
- host-specific operations are selected explicitly and are absent from unrelated targets;
- each target checks only the tools it actually uses.

MSYS2 still recommends UCRT64 when there is no contrary requirement. Its UCRT64 `PATH` begins
with `/ucrt64/bin:/usr/bin`. Native Windows tools therefore take priority, while recipes can use
the MSYS POSIX tools. This is a suitable model for TaskMate, but it does not remove the MSYS
runtime from shell utilities.

The largest unresolved distribution issue is still BSD `bmake`. The official MSYS2 package
index contains GNU Make but no `bmake` package. The original audit overstated the pkgsrc
evidence. The cited Windows MSYS2 report reached and built bootstrap `bmake`, but the complete
pkgsrc bootstrap later failed while building libarchive. It is evidence that a port can be made,
not a supported installation procedure that can be copied unchanged.

The original estimate of three to six days is optimistic. A realistic estimate is five to nine
days excluding hardware, with the largest uncertainty in building and packaging `bmake`. If a
qualified `bmake` artifact already exists, four to six days is reasonable.

## Status of the original findings

| Original finding | Current state | Evidence and required action |
| --- | --- | --- |
| Windows-forbidden `:` in log names | Resolved | `VAL_DATE_TIME` now uses `%Y_%m_%d_%Hh%Mm%Ss` |
| Global dependency check is too broad | Open | `.BEGIN` still checks one broad list and a reusable stamp |
| Hard-coded tools and paths | Partial | `/root/...` paths are gone, but Clang tool names remain hard-coded |
| Insufficient path contract | Open | Make lists remain whitespace-delimited and often unquoted |
| Line endings and script execution | Open | Tracked inputs are currently LF, but `.gitattributes` is absent |
| FreeBSD backup and serial paths | Partial | Backup data is guarded, but non-FreeBSD `backup` still prompts and succeeds as a no-op |
| Tool versions and reproducibility | Partial | The build report improved, but omits several host and AVR tool identities |

## Current build architecture

For `test1`, the current normal build performs these major operations:

1. `bmake` selects the target, board, MCU, and architecture fragments;
2. Make and AWK derive selected sources from target and `init.rc` declarations;
3. `.BEGIN` checks broad host and AVR program lists;
4. Clang builds autoCode 1.5 for the host;
5. autoCode validates `init.rc` syntax 1.10 and emits target-local fragments;
6. AWK enforces architecture and header boundaries;
7. `avr-gcc` compiles and links the firmware;
8. AVR tools and CLOC generate memory and source reports;
9. `avrdude` optionally flashes the Arduino Mega.

The structure is portable to a POSIX environment on Windows. The current risks are in host
selection, executable names, dependency checks, file semantics, path handling, and provisioning.

## Component compatibility

| Component | MSYS2/UCRT64 state | TaskMate action |
| --- | --- | --- |
| BSD `bmake` | Blocking, no official package | Build, package, pin, checksum, and test it |
| `sh`, AWK, `find`, `grep`, `sed`, Coreutils | Available from MSYS | Install explicit packages and keep `/usr/bin` in `PATH` |
| Host Clang | Available from UCRT64 | Select through a Make variable and record its version |
| `clang-format`, `clang-tidy` | Available with unversioned executable names | Remove the hard-coded `19` suffix or override it by role |
| AVR GCC, Binutils, avr-libc | Available as an official UCRT64 group | Pin and qualify a package set |
| `avrdude` | Available as an official UCRT64 package | Require an explicit Windows serial port |
| CLOC, Ctags, Doxygen, Cppcheck | Available | Install and check only for targets that use them |
| autoCode | Standard C host program | Model `.exe` explicitly and run Windows file tests |
| ASan and UBSan | Compiler support is available in principle | Qualify each UCRT64 runtime separately |
| `backup` | FreeBSD-only implementation | Fail clearly on unsupported hosts before prompting |
| Arduino upload | Feasible through native `avrdude.exe` | Validate with a physical board |

The official UCRT64 AVR group currently contains `avr-binutils`, `avr-gcc`, and `avr-libc`.
The official `avrdude` package installs `/ucrt64/bin/avrdude.exe`. These facts remove the need
to import an unrelated AVR distribution into the MSYS2 environment.

## Blocking and important repository gaps

### 1. There is no MSYS2 host model

`mk/options.mk` accepts only `freebsd`, `linux`, and `w10-cygwin`. Running an MSYS2 build as
`freebsd` would select incorrect peripheral behaviour. Reusing `w10-cygwin` would hide a real
tool and runtime boundary.

Add an explicit host such as `w10-msys2-ucrt64`, or replace the runtime-specific name with a
documented Windows POSIX host class plus an independently detected runtime. Tests must cover the
accepted value and reject incompatible combinations of `HOST`, `MSYSTEM`, and tool locations.

### 2. `bmake` provisioning is not solved

TaskMate uses `.include`, `.if`, `.for`, `.WAIT`, `.BEGIN`, `.MAKE.EXPAND_VARIABLES`, `:T`,
`:ts`, `!=`, suffix substitutions, and other BSD Make semantics. GNU Make is not a substitute.

The official MSYS2 repositories still do not provide `bmake`. The pkgsrc guide states that its
bootstrap installs `bmake`, but the cited MSYS2 report required local Msys platform files and
failed before the full bootstrap completed. The old command sequence based on a live pkgsrc clone
must therefore not be treated as a qualified TaskMate installation procedure.

The pilot must begin with one of these controlled approaches:

1. build standalone `bmake` from a pinned upstream source and package it for MSYS2;
2. extract and package the proven bootstrap `bmake` stage with all required platform patches;
3. maintain a TaskMate tool archive containing the executable, required make files, licence,
   source reference, build procedure, and SHA-256 checksum.

The resulting binary must pass a TaskMate dialect fixture before it is used for the real build.
The fixture must cover every BSD Make feature listed above and path values containing a drive
mount, spaces, and an executable suffix.

### 3. The current reference baseline is failing

The port cannot distinguish Windows defects from existing repository defects until the reference
host passes. On 3 October 2026, the following Cygwin validations failed:

- `bmake TARGET=test1 test_build_system` stopped in `mk/header_allow.mk` because the
  `exists()` condition for allowed architecture-type users is inverted;
- `bmake test_build_system` reached its configuration stage, then failed its
  `selected_target_autocode_test` for the same target parsing problem;
- `bmake test_autoCode` passed 22 cases in its first three stages, then failed
  `initrc/valid_commands` because the fixture lacks the required `driver_have` tag;
- a normal `test1` build is blocked by the same `mk/header_allow.mk` condition.

These are not MSYS2 failures. They must be corrected and shown to pass on the existing reference
host before any result is accepted as port evidence.

### 4. Program checks are broad, incomplete, and stale across environments

`conf/programs-list.conf` still includes CLOC, Ctags, `mount`, `umount`, and rsync in the
global list. It also includes all AVR tools even though an AVR-specific list and stamp exist.
Conversely, it omits Doxygen, Cppcheck, `clang-format19`, and `clang-tidy19` even though targets
invoke them.

Both host and AVR checks are attached through `.BEGIN`. Their stamps depend on list files, not on
the active `PATH`, `HOST`, `MSYSTEM`, executable identity, or tool version. A stamp produced by
one environment can suppress checks after switching environments.

Replace these checks with explicit prerequisites for these groups:

- minimum parser and normal firmware build tools;
- selected architecture compiler and report tools;
- host tests and sanitizers;
- formatting and static analysis;
- documentation and editor utilities;
- host-specific backup or upload tools.

A check may create a stamp only if the stamp records and validates the environment identity and
resolved executable paths. A non-destructive `doctor` target should always perform a fresh check.

### 5. Host executable and tool names are not abstracted consistently

The `/root/code/...` include paths identified by the original audit are gone. The current build
also defines variables for Clang-Tidy and Cppcheck. However:

- `mk/utils.mk` directly calls `clang-format19`;
- `FILE_CLANG_TIDY` defaults to `clang-tidy19`;
- `mk/autoCode.mk` directly calls `clang`;
- host output names omit the native `.exe` suffix;
- tests hard-code several of the same names and suffix-free paths.

Use role variables for the host C compiler, formatter, tidy tool, Cppcheck, and host executable
suffix. UCRT64 defaults should use `clang`, `clang-format`, `clang-tidy`, `cppcheck`, and `.exe`.
Keep the AVR `CC` and host compiler roles separate.

The suffix is not cosmetic. A native compiler can create `autoCode.exe` while Make tracks
`build/autoCode`. This can cause repeated rebuilding or a missing-target error even if the MSYS
shell can execute the suffix-free name.

### 6. Windows path support needs an explicit first-stage contract

The current checkout path, `C:\TaskMate_current`, avoids spaces. The repository still converts
`find` output into whitespace-separated Make lists and expands many lists without per-item
quoting. Supporting arbitrary Win32 names would require a wider redesign of discovery and Make
list handling.

For the first port, require:

- an ASCII checkout path without spaces, tabs, wildcard characters, or trailing dots;
- MSYS-style paths inside Make and shell commands;
- no Cygwin directories in the MSYS2 `PATH`;
- no Windows `find.exe`, `sort.exe`, or other homonyms before `/usr/bin`;
- a separate checkout or worktree and `build/` tree for each environment.

The `doctor` target must reject a violated contract instead of allowing a later partial build.

### 7. Line endings and native file replacement remain unqualified

The inspected tracked Make, shell, AWK, `init.rc`, error, GPIO, and list files currently use LF.
The repository has no `.gitattributes`, so this property depends on each Git installation.

Add a tracked `.gitattributes` with LF rules for at least `Makefile`, `*.mk`, `*.sh`, `*.awk`,
`*.rc`, `*.err`, `*.gpio`, and `*.list`. Allow that file through `mk/path_files.mk`, then
regenerate `.gitignore`. Do not edit the generated `.gitignore` directly. Tests must cover LF
shebangs and stable byte output after a second generation.

Native autoCode must also be tested for:

- text-mode newline behaviour;
- `.exe` discovery and execution;
- replacement of an existing destination with `rename()` on Windows;
- cleanup after open, write, close, remove, and rename failures;
- unchanged-file detection and preservation of modification times where expected.

The expanded autoCode tests are useful evidence, but the current suite does not pass and several
permission and `/dev/full` fixtures are Unix-specific. Windows equivalents or conditional fixture
implementations are required without weakening the asserted behaviour.

### 8. Backup and upload behaviour is only partially isolated

FreeBSD device and mount operations are now inside a FreeBSD conditional. This is an improvement.
The `backup` target still prints its destination, prompts for Enter, and then succeeds without
doing anything on a non-FreeBSD host. An unsupported destructive utility must fail before any
prompt with a clear diagnostic.

The Arduino port defaults to `/dev/ttyU0` only on FreeBSD. Other hosts leave it empty. Require an
explicit `VAL_PROGRAMMER_PORT=COMn` for Windows and validate that it is non-empty before invoking
`avrdude`. Upload must remain an explicit target and must never be part of automated host tests.

### 9. Reproducibility reporting remains incomplete

The current build report records TaskMate, hardware, Git, the selected target compiler, autoCode,
and `init.rc` versions. It does not identify `bmake`, MSYS runtime, host Clang, `avr-ld`,
`avr-libc`, `avrdude`, resolved executable paths, or the MSYS2 package manifest.

For a qualified build, record at least:

- Windows build, `MSYSTEM`, MSYS runtime, and effective `PATH` policy;
- `bmake` source revision, package version, and checksum;
- host Clang and sanitizer runtime versions;
- `avr-gcc`, `avr-ld`, `avr-libc`, Binutils, and `avrdude` versions;
- `pacman -Q` output for the selected package set;
- repository revision, dirty state, target, full hardware stack, and input format versions.

Equal firmware output is meaningful only when the compiler, linker, libraries, options, and
inputs match. With different tool versions, compare sections, symbols, disassembly, size, and
hardware behaviour instead of treating a HEX difference alone as a failure.

## Revised implementation procedure

### Phase 0: restore a passing reference

1. Correct the current build-system and autoCode test regressions in separate scoped work.
2. Run `test_build_system`, `test_autoCode`, `autoCode_alone`, and a clean `test1` build under
   the existing Cygwin reference.
3. Save tool versions, generated output, build report, ELF, HEX, section data, and size data.
4. Keep Cygwin installed and do not share its checkout or build artifacts with MSYS2.

The observed Cygwin environment is no longer missing the main build tools listed by the original
audit. Cygwin 3.6.10 provides the POSIX utilities and Clang, while AVR executables are available
from an external Windows toolchain. `clang-format19` and `clang-tidy19` were not found.

### Phase 1: prepare and isolate MSYS2 UCRT64

Install current 64-bit MSYS2 in its default location and update it fully:

```sh
pacman -Syu
```

If MSYS2 requests a terminal restart, close the terminal, reopen the UCRT64 profile, and repeat
the update. Validate the selected environment:

```sh
test "${MSYSTEM}" = UCRT64
printf '%s\n' "${PATH}"
```

The expected leading paths are `/ucrt64/bin:/usr/bin`. Do not add Cygwin directories. Use a new
checkout such as `/c/TaskMate_current_msys2` and retain the no-space path contract.

### Phase 2: install a minimal package set

Normal build and host-test prototype:

```sh
pacman -S --needed \
	base-devel bash coreutils diffutils findutils gawk grep sed git cloc \
	mingw-w64-ucrt-x86_64-clang \
	mingw-w64-ucrt-x86_64-avr-toolchain \
	mingw-w64-ucrt-x86_64-avrdude
```

Optional quality and documentation targets:

```sh
pacman -S --needed \
	ctags doxygen \
	mingw-w64-ucrt-x86_64-clang-tools-extra \
	mingw-w64-ucrt-x86_64-cppcheck
```

Do not install rsync or mount tools merely to satisfy the normal build. Capture `pacman -Q` after
qualification. Release builds need a controlled snapshot or archived package set, not only a
list of moving package names.

### Phase 3: qualify the packaged `bmake`

Before changing TaskMate, run the package in a standalone fixture that verifies:

- the exact BSD Make operators and directives used by the repository;
- `.WAIT` ordering and `.BEGIN` behaviour;
- shell selection and exit status propagation;
- MSYS drive paths and a checkout path contract violation;
- native `.exe` targets and incremental timestamp behaviour;
- `bmake -C`, `.CURDIR`, `.PARSEDIR`, and relative includes.

Reject an unknown package revision or checksum. Do not copy `bmake.exe` from Cygwin. A
Cygwin-linked executable keeps Cygwin runtime and path semantics and does not qualify MSYS2.

### Phase 4: apply repository adaptations

Recommended order:

1. add and test the MSYS2 UCRT64 host model;
2. abstract host compiler, quality tools, and executable suffixes;
3. split tool checks and remove environment-sensitive `.BEGIN` stamps;
4. add the line-ending contract;
5. reject unsupported backup execution before prompting;
6. validate an explicit Windows programming port;
7. add `doctor` and a complete version manifest;
8. add Windows variants for host file-failure tests.

Keep these changes in the build system and tests. Do not add Windows conditionals to firmware
code where standard C and host build selection are sufficient.

### Phase 5: validate without hardware

After the current reference defects and the port changes are fixed, run from UCRT64:

```sh
bmake HOST=w10-msys2-ucrt64 doctor
bmake HOST=w10-msys2-ucrt64 TARGET=test1 -V VAL_HW_STACK
bmake HOST=w10-msys2-ucrt64 TARGET=test1 -V FILES_COMPILE_SRC
bmake HOST=w10-msys2-ucrt64 test_build_system
bmake HOST=w10-msys2-ucrt64 test_autoCode
bmake HOST=w10-msys2-ucrt64 TARGET=test1 autoCode_alone
bmake HOST=w10-msys2-ucrt64 TARGET=test1
bmake HOST=w10-msys2-ucrt64 TARGET=test1 clean
bmake HOST=w10-msys2-ucrt64 TARGET=test1
```

Acceptance criteria:

- all commands return zero;
- `doctor` resolves only UCRT64 and MSYS executables from approved prefixes;
- no Windows homonym such as `C:\Windows\System32\find.exe` is selected;
- autoCode leaves no temporary files and its second pass changes no generated output;
- negative architecture and header fixtures still fail for the expected reason;
- clean operations remain confined below the selected `build/` directory;
- the firmware links for ATmega2560 and remains within flash and RAM limits;
- a second unchanged build performs no unnecessary host or firmware compilation;
- two clean builds with one package manifest produce equivalent results.

Run sanitizer tests separately and record whether the UCRT64 runtime supports every requested
sanitizer. A missing Windows sanitizer must not suppress ordinary functional tests. Keep a
sanitizer-qualified platform in CI.

### Phase 6: validate upload and performance

1. Identify the Arduino port in Device Manager.
2. Record `avrdude -v` and its selected configuration file.
3. build normally, then upload explicitly with `VAL_PROGRAMMER_PORT=COMn`;
4. test boot, timer, scheduler, GPIO, USART, and SCLI on the Mega 2560;
5. compare Cygwin and MSYS2 using the same source revision and qualified tool versions;
6. measure five clean and five incremental builds, including autoCode, compile, link, and CLOC.

Do not justify the port by assumed speed. MSYS POSIX utilities still use an emulation runtime.
Measure on the same host under the same antivirus policy.

## Alternative environments

| Approach | Feasibility | Main advantage | Main limitation | Decision |
| --- | --- | --- | --- | --- |
| Hybrid MSYS2/UCRT64 | Good after the listed work | Native current AVR and Windows upload tools | TaskMate must own `bmake` packaging | Pilot after prerequisites |
| Hardened Cygwin64 | Very good | Official `bmake` and known semantics | Mixed external AVR provisioning and measured performance concerns | Keep as reference and fallback |
| WSL2 Linux | Very good for build and tests | Straightforward Linux packages and CI | USB requires `usbipd-win`; Windows filesystem access is slower | Best fallback if `bmake` ownership is rejected |
| Linux container under WSL2 | Good for reproducible builds | Versioned dependency image | More awkward USB and interactive use | Good CI option |
| Full Linux or FreeBSD VM | Good | Close to a Unix reference | Administrative and editor overhead | Robust but heavy fallback |
| CMake and Ninja rewrite | Feasible as a separate project | Removes BSD Make distribution issue | Must reproduce the complete build contract | Do not combine with this port |
| GNU Make, Git Bash, or Scoop alone | Low | Small initial installation | Does not implement the current BSD Make graph | Reject without a rewrite |

Cygwin is now a stronger short-term option than the original audit reported because its main
program set is present on the observed host. Its baseline still needs the repository defects
listed above to be fixed.

WSL2 remains the best fallback when maintaining `bmake` for MSYS2 is unacceptable. Keep the
checkout in the Linux filesystem for build performance. Arduino access requires `usbipd-win` and
must be qualified separately, or flashing can remain a native Windows step.

A CMake or Ninja conversion remains a separate build-system project. It must reproduce target
composition, autoCode generation, dynamic dependencies, boundary checks, negative tests, AVR
dependency files, memory reports, and confined clean behaviour before replacing BSD Make.

## Risks and decisions

| Risk | Level | Required mitigation |
| --- | --- | --- |
| No official MSYS2 `bmake` | High | Pinned TaskMate package, checksum, source, and dialect tests |
| Failing current reference tests | High | Restore a green Cygwin baseline before port qualification |
| Native `.exe` and Windows file semantics | High | Explicit suffix model and Windows autoCode tests |
| Broad and reusable tool-check stamps | High | Target-scoped checks tied to environment identity |
| Paths and line endings | High before correction | Restricted checkout contract and generated attributes |
| AVR package version drift | High for releases | Archive a qualified package manifest and compare artefacts |
| Mixed MSYS2, Cygwin, and Windows `PATH` | High | Isolated shells, checkouts, and `doctor` enforcement |
| Unsupported backup behaviour | Medium | Fail before prompting on non-FreeBSD hosts |
| Serial upload differences | Medium | Explicit port and physical hardware test |
| Performance gain below expectations | Medium | Phase measurements before changing the reference host |
| Windows 10 end of support | High operational risk | Use ESU for the pilot or migrate the workstation |

Windows 10 22H2 left standard support on 14 October 2025. MSYS2 currently requires 64-bit
Windows 10 as its minimum Windows baseline and remains technically compatible with this host.
Compatibility does not remove the security and maintenance risk of downloading toolchain
packages on an operating system without standard security support. Operational acceptance
requires ESU or migration.

## Revised estimate

| Work package | Indicative effort |
| --- | ---: |
| Restore and capture the reference baseline | 0.5 to 1 day |
| Build, package, and test `bmake` | 1 to 2.5 days |
| Repository host, tool, suffix, and line-ending adaptations | 1.5 to 2.5 days |
| Host tests, determinism, and Cygwin comparison | 1.5 to 2 days |
| Manifests and operator procedure | 0.5 to 1 day |
| Arduino Mega upload and hardware acceptance | 0.5 day |

The estimate excludes a CMake rewrite, a Windows backup implementation, and unrelated functional
defects discovered during qualification.

## Recommended decision

Do not start by installing MSYS2 and modifying TaskMate in one uncontrolled environment. First
restore the green Cygwin baseline. In parallel, produce a pinned standalone `bmake` artifact and
make it pass the dialect fixture. These are the two entry criteria for an MSYS2 pilot branch.

Proceed with phases 1 to 5 only after both criteria pass. Adopt MSYS2 as the reference Windows
environment only if the complete host tests and firmware build pass reproducibly, the packaged
`bmake` has a maintainable update process, and measurements show a useful operational benefit.

If TaskMate should not own a `bmake` package, retain hardened Cygwin for native Windows work or
use WSL2 for builds and tests with a separate native Windows upload step.

## External sources

- [MSYS2 environments and UCRT64 path model](https://www.msys2.org/docs/environments/)
- [MSYS2 supported Windows versions](https://www.msys2.org/docs/windows_support/)
- [MSYS2 package management and package discovery](https://www.msys2.org/docs/package-management/)
- [MSYS package index, currently without bmake](https://packages.msys2.org/packages/?repo=msys)
- [Official UCRT64 AVR toolchain group](https://packages.msys2.org/groups/mingw-w64-ucrt-x86_64-avr-toolchain)
- [Official UCRT64 avrdude package](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-avrdude)
- [Official UCRT64 Clang tools package](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-clang-tools-extra)
- [pkgsrc bootstrap guide](https://www.netbsd.org/docs/pkgsrc/platforms.html)
- [MSYS2 UCRT64 pkgsrc bootstrap report and failure](https://mail-index.netbsd.org/pkgsrc-users/2024/03/23/msg039236.html)
- [Official Cygwin bmake package](https://cygwin.com/packages/summary/bmake.html)
- [Microsoft WSL USB and Arduino guidance](https://learn.microsoft.com/en-us/windows/wsl/connect-usb)
- [Windows 10 end-of-support notice](https://learn.microsoft.com/en-us/lifecycle/announcements/windows-10-end-of-support)
- [Windows 10 Extended Security Updates](https://learn.microsoft.com/en-us/windows/whats-new/extended-security-updates)

## Validation performed during this re-evaluation

- compared the current repository with the original audited revision;
- rechecked every original repository gap against current Makefiles, scripts, and tests;
- verified that relevant tracked text inputs currently contain no CR bytes;
- verified that `.gitattributes` and an MSYS2 host value are absent;
- checked official MSYS2 package data on 3 October 2026;
- corrected the interpretation of the cited pkgsrc MSYS2 report;
- confirmed that MSYS2 is absent from the observed host;
- recorded Windows build 19045.6466, Cygwin 3.6.10, and `bmake` 20240314;
- confirmed paths for AWK, Clang, AVR GCC, AVR LD, avrdude, CLOC, Ctags, Findutils, Git,
  rsync, and Sed under the current Cygwin login environment;
- confirmed that `clang-format19` and `clang-tidy19` are absent there;
- ran `test_build_system`, `test_autoCode`, and selected-target parsing under Cygwin and
  recorded their current failures;
- performed no MSYS2 installation, package change, firmware upload, backup, or hardware write.

Full feasibility validation still requires the packaged `bmake`, a passing reference baseline,
the UCRT64 pilot, and a physical Arduino Mega 2560 test.
