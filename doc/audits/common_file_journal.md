# Audit of a shared autoCode and build write journal

Date: 25 September 2026

Audited branch: `work`

Audited revision: `c45ecfce76c3aba33509913b27386b88d79ca04d`

Audit environment: Linux 6.18.44, without BSD `bmake`

## Purpose and requirement

This audit assesses a journal shared by autoCode and the Makefiles, checked at the end of the
build, to detect when a produced file was not completely written or properly closed. It covers
outputs created by the normal `test1` build, not every file read by `bmake`, the compiler, or
external utilities.

Three properties must be distinguished:

- **closed**: the producer requested closure and checked its result;
- **complete**: the content satisfies a contract known to the producer;
- **published**: only complete content is visible under the final name.

A journal can prove that an event was declared. By itself, it cannot prove that the expected
bytes exist, that buffers were durably written, or that a killed process had time to record its
failure. The requirement is therefore technically feasible for **managed and declared outputs**,
but not literally for "every open file".

## Verdict

The recommended solution is not a universal open-file registry. It is a **per-run publication
manifest**, shared by protocol between autoCode and the build, combined with temporary writes and
renaming. The final check verifies that every declared output has a terminal `committed` event,
that the final file satisfies its contract, and that no temporary file from the run remains.

Feasibility is high and cost remains moderate if the initial scope is limited to autoCode and
text files written directly by recipes. Complexity becomes high when objects, dependencies,
executables, and internal files from every tool are included. This extension would add little.
Successful process termination already ensures that the kernel closes its descriptors, but not
that its output is semantically valid.

A journal alone provides little reliability. It becomes valuable when combined with:

1. an exhaustive list of expected outputs;
2. checked closure before publication;
3. atomic publication by adjacent rename;
4. completeness criteria for each output type;
5. build failure if the final validator finds a missing or inconsistent state.

| Scope | Feasibility | Complexity | Reliability gain | Recommendation |
| --- | --- | --- | --- | --- |
| autoCode outputs | High | Low to medium | High with validation | Priority |
| Text produced by Makefiles | High | Medium | High with atomic writes | Priority |
| Objects, `.d`, ELF, and tool reports | Medium | High | Low to medium | Outside initial batch |
| All child-process reads and writes | Low | Very high | Low | Reject |

## Current state

### autoCode already provides most local control

All opens go through `fileOpen()` or `fileMakeTmp()`. `file_t` stores open state and write
permission (`srcs/autoCode/fileUtility.h:31-38`). `fileClose()` checks `ferror()`, checks
`fclose()`, then resets the object (`srcs/autoCode/fileUtility.c:208-228`). Generated
destinations are written to temporary files, closed, compared, and renamed
(`srcs/autoCode/fileUtility.c:65-101,289-302`).

This foundation makes internal control feasible without system interposition. Three limitations
remain for this requirement:

- several calls ignore the `fileClose()` result, especially the list files in `main`
  (`srcs/autoCode/autoCode.c:89-160`);
- each local object carries its own `stream_opened` boolean, with no registry of all live
  streams;
- renaming proves that a temporary file was published, not that the fragment is semantically
  complete.

The current summary counts only modified or unchanged destinations
(`srcs/autoCode/fileUtility.c:57-63`). It is not an account of opens and closes.

### Recipes share neither a process nor a file abstraction

Recipes construct several outputs using truncation with `>`, followed by appends with `>>`:

- autoCode configuration and lists (`mk/autoCode.mk:66-79,97-114`);
- `tm_info.h.tmp`, followed by comparison and rename (`mk/build.mk:18-40`);
- the build manifest written directly under its final name (`mk/build.mk:51-70`);
- dependency aggregation (`mk/build.mk:95-99`);
- `.gitignore`, architecture reports, statistics, and logs.

Each recipe line may start a new shell. `bmake` therefore cannot access command-internal file
descriptors, and state held in a shell variable does not necessarily survive into the next
recipe. Redirections are also opened by the shell before the called utility runs.

The build is declared `.NOTPARALLEL` (`mk/options.mk:16-17`), which simplifies a sequential
journal. Sub-makes and two separately launched builds remain distinct producers. The journal
must have a run identifier and must never be an unguarded reusable global file.

### The current final target cannot serve as proof

`.END` produces `last_build_info.txt` and the summary (`mk/build.mk:51-86`), while `all`
announces completion after its dependencies (`mk/build.mk:88-93`). A reliability validator
must be an explicit dependency placed after the producers, not only a cosmetic `.END` command.
Its status must directly affect the success of `all`, and it must not validate an old journal
when the build stops before reaching it.

## Limits of universal observation

### Closure

When a process terminates, the kernel closes its remaining open descriptors. A snapshot taken
after the build cannot distinguish a successful explicit close from automatic closure. It also
cannot reliably attribute an inherited, duplicated, or reopened descriptor.

`LD_PRELOAD` interposition would miss static binaries, direct system calls, and programs that
clear their environment. A system trace, such as `ktrace` or `truss` on FreeBSD, would be
large, host-specific, and difficult to correlate with `fork`, `exec`, `dup`, `rename`,
and shell-opened outputs. These techniques suit occasional diagnosis, not a deterministic build
contract.

### Completeness

Neither `close(2)` nor a `closed` event defines "complete". An empty file may be valid. A
non-empty file may be truncated but syntactically plausible. Each class therefore needs a
contract: number and names of autoCode fragments, list grammar, expected header and final line,
a `.d` file parser, or successful completion of the producing tool.

A hash recorded after closure detects later modification, but does not prove that the right
content was produced. `fsync()` would add durability after power loss, at notable host cost.
It is unnecessary for detecting a close error in a local build and must not be confused with
atomic publication.

## Recommended architecture

### 1. A shared protocol, not a shared library

C autoCode and shell recipes should not artificially share a library. They can produce the same
manifest format in a run-specific directory. Each terminal record should contain at least:

- protocol version and unique build identifier;
- producer and canonical destination path;
- output class and completeness criterion;
- close result, final size, and a digest if useful;
- terminal state: `committed`, `unchanged`, or `failed`.

A shared append-only event file would itself create the problem being controlled: concurrent
writes, partial lines, and locking. Each producer should instead atomically publish a small
record in `${PATH_BUILD_TARGET}/journal/<run-id>/`, after which the validator aggregates the
records. The final record name is the commitment. A temporary record alone means failure.

### 2. An expected inventory declared by the graph

The validator must compare commitments with a list prepared before writes begin. For autoCode,
the list comes from required fragments and destinations found while analysing tags. For Make,
it must explicitly list text files produced by the project itself. An unexpected record and an
expected record that is absent are both errors.

The recommended initial scope includes autoCode configuration and lists, generated fragments,
`tm_info.h`, `.deps.d`, architecture and header reports, size and LOC data, and the final
manifest. The autoCode stdout log may remain diagnostic because its completeness primarily
depends on redirection and process status. Individual `.o` and `.d` files, the autoCode
executable, and the firmware should retain their natural "successful recipe plus present
target" contract unless a later concrete defect justifies more.

### 3. Atomic publication before journalling

Managed Make outputs must use one helper: create a unique adjacent temporary file, write it in
one invocation, check status, verify the contract, then rename it. The helper then publishes
its commitment. A failure before the rename preserves the old destination and leaves a missing
or `failed` state for the validator.

autoCode should retain its C abstraction, but centrally register every open `file_t`, make all
close failures fatal before `rename()`, and publish commitments only after comparison or
replacement. An `atexit()` check can report streams still registered, but does not replace
explicit error paths and does not run after `SIGKILL` or machine shutdown.

### 4. Ordered validation and cleanup

An internal `_file_journal_check` target must depend on all relevant producers and precede the
`Build complete` message. It checks the identifier, completeness, uniqueness, terminal states,
class-specific criteria, confined paths, and absence of temporary files associated with the run.

The journal is marked `complete` only after this validation. At the next start, an incomplete
run is archived for diagnosis or removed in a confined manner. It is never accepted as proof of
the new build. A no-op build must either create and validate a new run, or verify a versioned
final manifest whose outputs remain consistent.

## Findings and priorities

### P1: "every open file" is not a verifiable contract

Without scope and a definition of completeness, the implementation would produce many traces
without useful evidence. It could even create false assurance because a balanced `open/close`
pair says nothing about content. The first decision must limit the contract to outputs written
and owned by TaskMate during `all`.

### P1: the journal does not fix direct publication

Several recipes write their destinations in place. A terminal journal can detect some failures,
but cannot restore previous content. Atomic writing is a prerequisite for the claimed
reliability gain, not an independent improvement to defer.

### P1: final validation alone does not cover interruptions

If the build is interrupted, the final target does not run. The next invocation must recognise
the incomplete run before reusing its stamps or outputs. A run identifier and publication of a
final marker are therefore mandatory.

### P2: autoCode can guarantee its own streams at low cost

The central path through `fileUtility` makes it possible to add a bounded or host-dynamic
registry, open identifiers, and a final report. The main work is strict propagation of close
results and fault-injection tests, not journalling itself.

### P2: including external tools would greatly increase cost

Clang, AVR GCC, `awk`, `cloc`, Git, and the shell have their own files and strategies.
Wrapping them at system level would make the build depend on FreeBSD details and tool-version
behaviour. Their exit status, declared targets, and focused format validators provide a better
cost-to-reliability ratio.

## Estimated implementation plan

### Step 1: contract and prototype, low complexity, 1 to 2 days

1. Define scope, versioned format, states, and criteria for each class.
2. Generate one identifier per `all` invocation and an expected inventory in `build/`.
3. Prototype the validator on synthetic manifests without changing producers.

### Step 2: autoCode, medium complexity, 2 to 4 days

1. Register open streams and require every close to be checked.
2. Emit an atomic commitment after each validated publication or retention.
3. Inject open, write, close, rename, and interruption failures.

This step is system-critical because it changes the generator. It requires the existing normal
and sanitizer tests, followed by a review of generated outputs.

### Step 3: Make producers, medium to high complexity, 3 to 6 days

1. Create and test the atomic-write and commitment helper.
2. Migrate text outputs in small groups without reformatting the Makefiles.
3. Add `_file_journal_check` to the `all` graph with explicit ordering.
4. Migrate external-tool outputs only when justified by a concrete failure mode.

### Step 4: robustness, medium complexity, 2 to 4 days

Test a full disk, close failure, signal, leftover temporary file, duplicate record, old run,
destination modified after commitment, and two concurrent builds. Also check an incremental
build without regeneration and a failure before the final validator.

The total estimate is 8 to 16 days, depending on the number of Make outputs included.
Intercepting every system call would far exceed this cost and require continuous maintenance,
without proving completeness.

## Proposed acceptance criteria

- no successful `all` without an inventory and final marker for the same run identifier;
- exactly one terminal commitment for each expected output;
- no publication if writing, validation, or closure fails;
- no previous destination changed after a failure before rename;
- no temporary file from the run after success, and reliable detection after interruption;
- no old journal accepted by a new build;
- tests compatible with the official FreeBSD and `bmake` platform;
- no requirement added to embedded firmware because the mechanism remains entirely host-side.

## Validation performed during the audit

- read the `build` and `autoCode` architecture notes, rules, and existing safety audit;
- statically inventoried `fopen` and `fclose` calls, autoCode wrappers, and their callers;
- statically inventoried redirections, creations, moves, and removals in `mk/` and
  `scripts/`;
- inspected the `all` graph, `.BEGIN`, `.END`, `.NOTPARALLEL`, and declared outputs;
- verified the audited branch and revision.

BSD `bmake` was not installed in the Linux audit environment. No graph expansion or `test1`
build was run. No generated file or build value was changed. This design report requires no
validation on physical hardware.

## Conclusion

A shared journal is technically relevant if it becomes a **verifiable publication** protocol,
not a descriptor tracer. The best compromise is to cover autoCode and Makefile-owned text
outputs first, with a prior inventory, adjacent temporary files, checked closure, atomic rename,
and a commitment from each producer.

This approach provides a large reliability improvement against partial files, ignored close
failures, and interrupted runs. It remains independent of firmware and consistent with the
FreeBSD and `bmake` choice. Extending journalling to every file used by every tool would be
costly, fragile, and unable to prove completeness. That approach is not recommended.
