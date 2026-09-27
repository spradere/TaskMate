# Windows 10, MSYS2, and UCRT64 build port audit

Date: 16 September 2026

Audited branch: `test`

Audited revision: `d0fabace2babc6fb61828d30b737ea0125c6639e`

Observed host: Windows 10 Pro 22H2, build 19045, Cygwin 3.6.6, `bmake` 20240314

## Purpose and scope

This audit assesses the technical feasibility and procedure for porting the TaskMate build
system to Windows 10 with MSYS2 and its UCRT64 environment. It covers BSD `bmake`
orchestration, autoCode, shell and AWK scripts, AVR compilation, host tests, quality tools,
flashing an Arduino Mega, and supporting utilities.

The RTOS code and embedded architecture do not require a Windows port. Only the build host
changes. The target remains `avr8 / atmega2560 / Arduino Mega` and must retain the same
compilation options, boundary checks, generated data, and deterministic constraints.

The audit is based on the project rules, `doc/architecture/build.md`,
`doc/architecture/autoCode.md`, the `Makefile`, `mk/*.mk`, AVR fragments, scripts, and
tests. It did not install MSYS2 or modify the build.

## Verdict

**The MSYS2/UCRT64 port is feasible, with medium technical risk and an estimated three to six
days of development and validation excluding hardware.** It does not work without changes in
the current repository.

The recommended solution is hybrid:

- shell, AWK, and POSIX utilities come from the MSYS subsystem;
- Clang, the AVR toolchain, and `avrdude` are native UCRT64 Windows executables;
- BSD `bmake` is retained and provisioned as a pinned project tool;
- strictly FreeBSD targets, especially `backup`, are isolated from the normal build;
- a diagnostic command checks tools required by the requested target, not every possible
  utility.

MSYS2 recommends UCRT64 and constructs its `PATH` as `/ucrt64/bin:/usr/bin`. Native Windows
tools therefore take priority while the POSIX recipes retain the required MSYS commands. This
model suits TaskMate. MSYS2 does not remove every emulation layer because its `/usr/bin` tools
still use a Cygwin-derived runtime. Any performance gain over Cygwin must be measured.

One distribution obstacle remains. At audit time, the MSYS2 repository has no `bmake` package.
GNU Make cannot interpret the current `.include`, `.if`, `.for`, `.WAIT`, `:T`, `:ts`,
`!=`, and suffix transformations without a rewrite. The documented pkgsrc bootstrap on MSYS2
installs `bmake` and is acceptable for a proof of concept. For long-term use, TaskMate should
provide an internal `bmake` package or archive with a fixed version and checksum.

## Current build architecture

The complete build currently follows this sequence:

1. `bmake` reads the target, board, MCU, and architecture composition;
2. a `.BEGIN` target checks a global program list and creates build directories;
3. `find` discovers files, and the shell calculates dynamic lists;
4. Clang builds autoCode for the host, then autoCode generates contract fragments;
5. AWK runs header and architecture checks;
6. `avr-gcc` compiles and links the firmware;
7. `avr-size`, AWK, and CLOC produce the report;
8. `avrdude` optionally flashes the board.

This structure can be ported to a POSIX environment on Windows. Generated formats and embedded
code use no Windows API. The main risks concern tool availability, Windows file names, paths,
and peripheral targets.

## Component compatibility

| Component | State under MSYS2/UCRT64 | Action |
| --- | --- | --- |
| BSD `bmake` | Blocking: absent from the official MSYS2 repository | Bootstrap pkgsrc for the prototype, then use a pinned internal package |
| `sh`, AWK, `find`, `grep`, `sed`, Coreutils | Available in the MSYS repository | Install packages explicitly and keep `/usr/bin` after `/ucrt64/bin` |
| Host Clang | Available in UCRT64 | Install `mingw-w64-ucrt-x86_64-clang` |
| `clang-format`, `clang-tidy` | Available, with no guaranteed version suffix | Use tool variables and unversioned names |
| AVR GCC, Binutils, avr-libc | Complete official UCRT64 group | Install `mingw-w64-ucrt-x86_64-avr-toolchain` |
| `avrdude` | Official UCRT64 package | Install separately and configure a `COMn` port |
| CLOC, Ctags, Doxygen, Cppcheck, rsync | Available, but not required by every build | Check only in targets that use them |
| autoCode | Portable C built for the host | Test renames, line endings, and `.exe` suffixes |
| Host ASan/UBSan | Must be qualified with the selected Clang | Keep optional until the runtime is validated |
| `backup` | Incompatible: FreeBSD device and mount | Disable on Windows or design a separate safe Windows target |
| Arduino upload | Feasible with `avrdude.exe` | Make the port configurable and test on hardware |

The official UCRT64 AVR group includes `avr-binutils`, `avr-gcc`, and `avr-libc`.
ATmega2560 is present in the GCC specifications. The `avrdude` package provides a native
executable under `/ucrt64/bin`.

## Blocking or important repository gaps

### 1. Log name forbidden on Windows

`mk/sources.mk:35` builds `VAL_DATE_TIME` with `%H:%M:%S`, then includes it in the
autoCode log name. Win32 forbids `:` in file names. Use, for example,
`%Y_%m_%d_%H-%M-%S`. This portable fix changes no embedded data.

### 2. Global dependencies are too broad

`conf/programs-list.conf` requires CLOC, Ctags, `mount`, `umount`, and rsync before a normal
build, although they support optional features. This explains some missing-program errors under
Cygwin and would reproduce them under MSYS2.

Separate at least:

- tools required to parse the Makefile and build firmware;
- host-test tools;
- quality and documentation tools;
- editing tools;
- platform-specific backup tools.

The check should be an explicit prerequisite of relevant targets, not a global `.BEGIN` side
effect. Its stamp must include or verify the active environment because an old stamp currently
survives a `PATH` change.

### 3. Hard-coded tools and paths

`mk/utils.mk` calls `clang-format19` and `clang-tidy19`, while UCRT64 provides the usual
`clang-format` and `clang-tidy` names. Cppcheck and Clang-Tidy commands also contain
`/root/code/TaskMate/TaskMate_current`, which is unrelated to the Windows checkout.

Define role variables such as `FORMAT`, `TIDY`, and `CPPCHECK`, with unversioned defaults,
then use only `${.CURDIR}` and `${PATH_SRCS}` for includes. The documented
`clang_format` target also differs from the real `format` target. Keep one name or add an
alias.

### 4. Insufficient path contract

`find` output becomes space-separated Make lists, and many recipes expand these lists without
quotes. Internal repository names are simple, but the observed absolute checkout path contains
`PC HP`. Relative paths reduce the issue without eliminating it, especially for
compiler-produced dependencies and `bmake -C`.

For the first port, use an ASCII path without spaces, such as
`C:\work\TaskMate_current`, and make this an explicit contract. Immediate support for all
Win32 and POSIX names would be disproportionate.

### 5. Line endings and script execution

Scripts with `#!/bin/sh` must remain LF. Git conversion to CRLF can invalidate a shebang or
file comparison. The repository should define line endings for `*.sh`, `*.awk`, `*.mk`,
`Makefile`, `*.rc`, `*.err`, `*.gpio`, and `*.list` in `.gitattributes`, without
depending on each host's global `core.autocrlf`.

The native autoCode binary must also be qualified for the `.exe` suffix, text-mode opening,
produced line endings, `rename()` over an existing file, and stable compare-and-replace.
Existing tests cover the functional logic well and should become the Windows compatibility
evidence.

### 6. FreeBSD targets and peripherals

`PATH_USBKEY=/media/usbkey`, `FILE_USBDEV=/dev/da0s1`, and `mount -t msdosfs` are
FreeBSD-specific. The `backup` target also uses `rsync --delete`. It must never be adapted
by simple path substitution. Make it unavailable on Windows with a clear diagnostic, or replace
it with a separate Windows implementation that validates the canonical volume, requires a
sentinel, and runs a preview first.

The `/dev/ttyU0` serial port is also specific. `VAL_PROGRAMMER_PORT` must be overrideable,
for example with `bmake upload VAL_PROGRAMMER_PORT=COM3`. Flashing must remain an explicit
hardware step outside initial automated validation.

### 7. Tool versions and reproducibility

The UCRT64 AVR package evolves independently of the historical environment. A new GCC version
can change diagnostics, size, LTO, and the final binary without a source change. The port must
record at least the versions of `bmake`, `clang`, `avr-gcc`, `avr-ld`, `avr-libc`, and
`avrdude`. Pin an MSYS2 snapshot or qualified version list for releases.

Comparing only HEX files between Cygwin and MSYS2 is insufficient if compiler versions differ.
Qualification should first compare equal versions, then inspect size, sections, symbols,
disassembly, and board behaviour.

## Recommended procedure

### Phase 0: retain a rollback path

1. Keep the current Cygwin installation until MSYS2 is fully validated.
2. Record its tool versions, a clean build, memory reports, HEX file, and reference autoCode log.
3. Do not share one `build/` directory between Cygwin and MSYS2. Compare two checkouts or
   worktrees.

The observed Cygwin installation is incomplete. With an explicit POSIX `PATH`, it lacks at
least AWK, Clang, Ctags, Findutils, Git, rsync, Sed, and the entire AVR toolchain, including
`avrdude`. The `bmake` package is installed. The current failure therefore comes largely
from provisioning and global tool checks, not a fundamental Cygwin limitation.

### Phase 1: install MSYS2/UCRT64

On Windows 10 22H2, install MSYS2 in `C:\msys64`, update the system, close and reopen the
terminal if requested, then repeat the update:

```sh
pacman -Syu
pacman -Syu
```

Always start the **UCRT64** profile and check:

```sh
test "${MSYSTEM}" = UCRT64
printf '%s\n' "${PATH}"
```

The expected `PATH` prefix is `/ucrt64/bin:/usr/bin`. Do not globally add Cygwin directories
to the MSYS2 `PATH`. Mixing DLLs, shells, and path conventions makes diagnostics
non-reproducible.

### Phase 2: install packages

Proposed initial prototype set:

```sh
pacman -S --needed \
	base-devel bash coreutils diffutils findutils gawk grep sed git rsync cloc ctags doxygen \
	mingw-w64-ucrt-x86_64-clang \
	mingw-w64-ucrt-x86_64-clang-tools-extra \
	mingw-w64-ucrt-x86_64-cppcheck \
	mingw-w64-ucrt-x86_64-avr-toolchain \
	mingw-w64-ucrt-x86_64-avrdude
```

After qualification, obtain the final manifest with `pacman -Q` and archive it with the build
report. Editors and backup tools must not enter the minimal base set.

### Phase 3: provision BSD `bmake`

For the proof of concept, use the unprivileged pkgsrc bootstrap in a path without spaces. The
NetBSD guide states that it installs `bmake`, and a Windows 10/11 report for MSYS2 UCRT64
confirms this path.

Principle:

```sh
git clone --depth 1 https://github.com/NetBSD/pkgsrc.git /opt/pkgsrc
cd /opt/pkgsrc/bootstrap
./bootstrap --unprivileged --prefix=/opt/taskmate-pkg
export PATH=/opt/taskmate-pkg/bin:/ucrt64/bin:/usr/bin
bmake -V MAKE_VERSION
```

This live clone is unsuitable for a durable release chain. After the prototype:

1. select a validated `bmake` version;
2. retain the exact sources and licence;
3. build an internal MSYS2 package or tool archive;
4. publish the SHA-256 checksum and reproducible procedure;
5. explicitly test `.WAIT`, `.MAKE.EXPAND_VARIABLES`, `:T`, `:ts`, `!=`, `.for`, and
   `.include`;
6. reject an unknown version in the diagnostic command.

### Phase 4: apply minimal repository adaptations

Recommended order:

1. remove colons from the log name;
2. introduce host-tool variables and remove `/root/...` paths;
3. split program manifests by target and remove the `.BEGIN` check;
4. declare Git line endings;
5. make the programming port overrideable;
6. declare `backup` unsupported on Windows;
7. add a non-destructive `doctor` target that reports the environment, paths, and versions;
8. record a version manifest in `build/`.

These changes must remain in the build system. Do not add `#if Windows` to firmware code or
autoCode where standard C is sufficient.

### Phase 5: validate without hardware

From a checkout without spaces and with `MSYSTEM=UCRT64`:

```sh
bmake doctor
bmake -V VAL_HW_STACK
bmake -V FILES_SRC
bmake test_build_system
bmake test_autoCode
bmake test_tm_string
bmake autoCode_alone
bmake
bmake clean
bmake
```

Acceptance criteria:

- every command returns zero, and no Windows homonym such as Windows `find.exe` is selected;
- autoCode tests leave no `.tmp`, and a second pass changes no output;
- architecture and header checks fail correctly on an invalid fixture;
- `clean` remains confined under `build/` with MSYS paths;
- firmware links for ATmega2560, with sections and RAM within limits;
- a second unchanged build does not rebuild autoCode or firmware unnecessarily;
- results match between two clean MSYS2 builds using the same tool manifest.

Qualify the sanitizer target separately. Its temporary absence must not hide ordinary test
failures, but it must remain mandatory on at least one supported CI platform.

### Phase 6: validate hardware and performance

1. Identify the Arduino port in Device Manager.
2. Run `avrdude -v` and save its version and configuration.
3. Flash explicitly with `VAL_PROGRAMMER_PORT=COMn`.
4. Test startup, timer, scheduler, GPIO, and USART/SCLI on the Mega 2560.
5. Compare Cygwin and MSYS2 using five clean and five incremental builds on the same machine
   with the same antivirus. Measure total, autoCode, compile, link, and CLOC times.

A performance improvement must not rely on globally disabling antivirus for the development
directory. If antivirus dominates measurements, any exclusion requires an explicit, limited
local security decision.

## Other possible approaches

| Approach | Feasibility | Advantages | Limits | Recommendation |
| --- | --- | --- | --- | --- |
| Hybrid MSYS2/UCRT64 | Good after adaptations | Recent packages, native AVR and avrdude, direct Windows integration | `bmake` must be provisioned; POSIX commands still use MSYS runtime | **Recommended for the pilot** |
| Hardened Cygwin64 | Very good, minimal effort | Official `bmake` package; known POSIX semantics | Observed slowness, incomplete current installation, toolchain to install | Excellent safety net and short-term solution |
| WSL2 Linux | Very good for build and test | Complete Linux packages, natural CI environment, good performance in Linux FS | No native USB, requires `usbipd-win`; `/mnt/c` is slower; VM layer | **Best alternative if MSYS2 `bmake` bootstrap is rejected** |
| Linux container under WSL2/Docker | Good for reproducible builds | Versioned image, strong dependency isolation | Awkward USB and flashing, potentially slow Windows volume, extra complexity | Good CI candidate, weaker interactive embedded workstation |
| Full Linux or FreeBSD VM | Good | Close to the reference Unix environment, possible USB pass-through | Administration, storage, startup, and weaker editor integration | Robust but heavy fallback |
| Native CMake and Ninja rewrite | Feasible in the medium term | Strong portability, native Windows speed, broad IDE and CI support | Graph rewrite, temporary dual maintenance, risk to autoCode and checks | Future study, not a prerequisite |
| GNU Make, Git Bash, or Scoop alone | Low without a rewrite | Lightweight installation | GNU Make is incompatible with BSD Makefiles; incomplete, scattered tools | Not recommended |

### Cygwin64 can be repaired immediately

Cygwin officially provides `bmake`. Other observed omissions can be installed with
`setup-x86_64.exe -P ...`. A quick action is therefore to create a versioned Cygwin package
list and split program checks. This does not solve observed intrinsic slowness, but provides a
reliable reference during the MSYS2 pilot.

Do not copy `bmake.exe` from Cygwin into MSYS2. It depends on the Cygwin runtime and would
launch tools with its conventions. Two complete, separate environments are safer than mixed
executables.

### WSL2 is the best fallback

The observed host, build 19045, meets the modern WSL minimum. Clone the repository in the Linux
file system, not under `/mnt/c`, to avoid cross-file-system access costs. The AVR toolchain,
Clang, tests, and `bmake` are simple to provision there.

Flashing requires `usbipd-win` because USB is not exposed natively to WSL. Microsoft
explicitly documents it for scenarios such as Arduino flashing. On Windows 10, validate the
Store version of WSL and the required kernel before making it an operator solution.

### Do not confuse CMake/Ninja with the host port

A CMake conversion could eventually remove the `bmake` dependency and most discovery
scripts. It must reproduce hardware composition, startup order, autoCode generation, dynamic
dependencies, boundary checks, AVR dependency files, memory reports, negative tests, and clean
rules exactly.

This is a separate build project requiring coexistence and differential tests. Doing it with
the environment change would multiply possible causes of differences. First qualify the
existing BSD build under MSYS2 or WSL2, then use measurements to decide whether a native rewrite
is worthwhile.

## Risks and decisions

| Risk | Level | Recommended mitigation |
| --- | --- | --- |
| MSYS2 does not provide `bmake` | High | Pinned internal package, checksum, and dialect test |
| Windows names, paths, and line endings | High before correction | Timestamp without `:`, checkout without spaces, `.gitattributes`, autoCode corpus |
| AVR version drift | High for an embedded release | Manifest and qualification of one precise version |
| Speed gain below expectations | Medium | Benchmark each phase before abandoning Cygwin |
| Mixed MSYS, Cygwin, and Windows `PATH` | High | Isolated environments and `doctor` target |
| Different serial flashing | Medium | Configurable port and explicit hardware test |
| Destructive or incompatible `backup` target | High | Make it unavailable on Windows |
| Windows 10 outside standard support | High security risk | ESU or Windows 11 migration, even if the build remains feasible |

Windows 10 22H2 reached the end of standard support on 14 October 2025. MSYS2 remains
technically compatible, but an Internet-connected toolchain that downloads packages and sources
should not be maintained on a host without ESU. This life-cycle issue is independent of port
success, but must be part of operational acceptance.

## Estimate

| Work package | Indicative effort |
| --- | ---: |
| MSYS2 installation, packages, and `bmake` prototype | 0.5 to 1 day |
| Minimal build fixes and `doctor` target | 1 to 2 days |
| Host tests, determinism, and Cygwin comparison | 1 to 2 days |
| Internal package, documentation, and manifests | 0.5 to 1 day |
| Arduino Mega flashing and acceptance | 0.5 day |

The estimate excludes a CMake/Ninja rewrite, a new Windows backup system, and fixes for
pre-existing functional defects exposed by tests.

## Recommended decision

Start an MSYS2/UCRT64 pilot on a dedicated porting branch without removing Cygwin. Decide after
phases 1 to 5. If firmware and all host tests pass with packaged `bmake`, and the benchmark
shows a useful improvement, MSYS2 becomes the reference Windows environment. If maintaining a
`bmake` package is too costly or the gain is small, use WSL2 for the build and keep a separate
native Windows flashing tool.

In the very short term, fixing Cygwin provisioning remains useful. It provides a complete
comparison baseline and can unblock the project before the pilot ends.

## External sources

- [MSYS2 environments and UCRT64 recommendation](https://www.msys2.org/docs/environments/)
- [MSYS2 package management](https://www.msys2.org/docs/package-management/)
- [MSYS repository packages and current absence of bmake](https://packages.msys2.org/packages/?repo=msys)
- [Official UCRT64 AVR group](https://packages.msys2.org/groups/mingw-w64-ucrt-x86_64-avr-toolchain)
- [Official UCRT64 avr-gcc package](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-avr-gcc)
- [Official UCRT64 avrdude package](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-avrdude)
- [Clang-Tidy and UCRT64 Clang tools](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-clang-tools-extra)
- [pkgsrc bootstrap on a non-NetBSD platform](https://www.netbsd.org/docs/pkgsrc/platforms.html)
- [pkgsrc bootstrap report on Windows and MSYS2 UCRT64](https://mail-index.netbsd.org/pkgsrc-users/2024/03/23/msg039236.html)
- [Official Cygwin bmake package](https://cygwin.com/packages/summary/bmake.html)
- [WSL installation and Windows 10 requirements](https://learn.microsoft.com/en-us/windows/wsl/install)
- [Connecting USB to WSL with usbipd-win](https://learn.microsoft.com/en-us/windows/wsl/connect-usb)
- [Windows 10 life cycle](https://learn.microsoft.com/en-us/lifecycle/faq/windows)

## Validation performed during the audit

- inventoried direct dependencies in Make, shell, AWK, and AVR fragments;
- checked official MSYS2 packages available on 16 September 2026;
- confirmed that MSYS2 was absent from the host, without installation or system changes;
- read the installed Cygwin and `bmake` versions;
- ran the Cygwin program checker non-destructively with an explicit POSIX `PATH`;
- checked the Windows 10 build 19045 system;
- performed no autoCode generation, firmware compilation, removal, backup, or hardware write.

Full feasibility validation still requires the pilot described above and a physical test on an
Arduino Mega 2560.
